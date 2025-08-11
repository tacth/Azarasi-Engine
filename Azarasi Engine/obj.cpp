#include <Windows.h>
#include <stdio.h>
#include <GL/glut.h>

#include "obj.h"

// バッファ長
#define BUFFER_LENGTH 1024
#define MAX_VECTOR_SIZE 10000
#define MAX_TRIANGLE_SIZE 10000
#define MAX_MTL_SIZE 32

//
// Objファイルの読み込み
//
Obj* LoadObj(const char* filename)
{
	FILE* fp;
	char line[BUFFER_LENGTH];
	char name[BUFFER_LENGTH] = { 0 };
	int i, j;
	Vector vec;
	int v_no[4], vt_no[4], vn_no[4];
	int count;
	Mtl* curr_mtl = NULL;

	// ファイルを開く
	errno_t err = fopen_s(&fp, filename, "r");
	if (err != 0 || fp == NULL)
	{
		return NULL;
	}

	// Obj構造体を初期化
	Obj* obj = new Obj();
	obj->num_vertices = 0;
	obj->num_normals = 0;
	obj->num_tex_coords = 0;
	obj->vertices = new Vector[MAX_VECTOR_SIZE];
	obj->normals = new Vector[MAX_VECTOR_SIZE];
	obj->tex_coords = new Vector[MAX_VECTOR_SIZE];
	obj->num_triangles = 0;
	obj->tri_v_no = new int[MAX_TRIANGLE_SIZE * 3];
	obj->tri_vn_no = new int[MAX_TRIANGLE_SIZE * 3];
	obj->tri_vt_no = new int[MAX_TRIANGLE_SIZE * 3];
	obj->tri_material = new Mtl * [MAX_TRIANGLE_SIZE];
	obj->num_materials = 0;
	obj->materials = NULL;

	// ファイルから1行ずつ読み込み
	while (fgets(line, BUFFER_LENGTH, fp) != NULL)
	{
		// マテリアルの読み込み
		if (strncmp(line, "mtllib", 6) == 0)
		{
			// テキストを解析
			sscanf_s(line, "mtllib %s", name, (unsigned)_countof(name));
			
			// 指定されたファイル名のマテリアルデータを読み込み
			if (strlen(name) > 0)
			{
				LoadMtl(name, obj);
			}
		}

		// マテリアルの変更
		if (strncmp(line, "usemtl", 6) == 0)
		{
			// テキストを解析
			sscanf_s(line, "usemtl %s", name, (unsigned)_countof(name));

			// 指定された名前のマテリアルデータを探索して記録
			for (i = 0; i < obj->num_materials; i++)
			{
				if (strcmp(name, obj->materials[i]->name) == 0)
				{
					curr_mtl = obj->materials[i];
					break;
				}
			}
		}

		// 頂点データの読み込み
		if (line[0] == 'v')
		{
			// 法線ベクトル（vn）
			if (line[1] == 'n')
			{
				// テキストを解析
				sscanf_s(line, "vn %f %f %f", &vec.x, &vec.y, &vec.z);

				// 法線ベクトル配列の末尾に格納
				if (obj->num_normals < MAX_VECTOR_SIZE)
				{
					obj->normals[obj->num_normals] = vec;
					obj->num_normals++;
				}
			}
			// テクスチャ座標（vt）
			else if (line[1] == 't')
			{
				// テキストを解析
				sscanf_s(line, "vt %f %f %f", &vec.x, &vec.y, &vec.z);

				// テクスチャ座標配列の末尾に格納
				if (obj->num_tex_coords < MAX_VECTOR_SIZE)
				{
					obj->tex_coords[obj->num_tex_coords] = vec;
					obj->num_tex_coords++;
				}
			}
			// 頂点座標（v）
			else
			{
				// テキストを解析
				sscanf_s(line, "v %f %f %f", &vec.x, &vec.y, &vec.z);

				// 法線ベクトル配列の末尾に格納
				if (obj->num_vertices < MAX_VECTOR_SIZE)
				{
					obj->vertices[obj->num_vertices] = vec;
					obj->num_vertices++;
				}
			}
		}

		// ポリゴンデータの読み込み
		if (line[0] == 'f')
		{
			// Obj形式ではインデックスの番号は1から始まるので、-1して0から始まるようにする
			for (j = 0; j < 4; j++)
			{
				v_no[j] = vt_no[j] = vn_no[j] = -1;
			}

			// テキストを解析
			count = sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
				&v_no[0], &vt_no[0], &vn_no[0],
				&v_no[1], &vt_no[1], &vn_no[1],
				&v_no[2], &vt_no[2], &vn_no[2]);

			// 頂点・テクスチャ・法線ありの場合
			if (count == 9)
			{
				for (int k = 0; k < 3; k++)
				{
					obj->tri_v_no[obj->num_triangles * 3 + k] = v_no[k] - 1;
					obj->tri_vt_no[obj->num_triangles * 3 + k] = vt_no[k] - 1;
					obj->tri_vn_no[obj->num_triangles * 3 + k] = vn_no[k] - 1;
				}
				obj->tri_material[obj->num_triangles] = curr_mtl;
				obj->num_triangles++;
			}
			// 頂点・法線ありの場合
			else
			{
				// テキストを解析
				count = sscanf_s(line, "f %d//%d %d//%d %d//%d",
					&v_no[0], &vn_no[0],
					&v_no[1], &vn_no[1],
					&v_no[2], &vn_no[2]);

				if (count == 6)
				{
					for (int k = 0; k < 3; k++)
					{
						obj->tri_v_no[obj->num_triangles * 3 + k] = v_no[k] - 1;
						obj->tri_vn_no[obj->num_triangles * 3 + k] = vn_no[k] - 1;
					}
					obj->tri_material[obj->num_triangles] = curr_mtl;
					obj->num_triangles++;
				}
				// 頂点・テクスチャありの場合
				else
				{
					// テキスト解析
					count = sscanf_s(line, "f %d/%d %d/%d %d/%d",
						&v_no[0], &vt_no[0],
						&v_no[1], &vt_no[1],
						&v_no[2], &vt_no[2]);

					if (count == 6)
					{
						for (int j = 0; j < 3; j++)
						{
							obj->tri_v_no[obj->num_triangles * 3 + j] = v_no[j] - 1;
							obj->tri_vt_no[obj->num_triangles * 3 + j] = vt_no[j] - 1;
						}
						obj->tri_material[obj->num_triangles] = curr_mtl;
						obj->num_triangles++;
					}
					// 頂点のみの場合
					else
					{
						// テキスト解析
						count = sscanf_s(line, "f %d %d %d",
							&v_no[0], 
							&v_no[1], 
							&v_no[2]);

						if (count == 3)
						{
							for (int j = 0; j < 3; j++)
							{
								obj->tri_v_no[obj->num_triangles * 3 + j] = v_no[j] - 1;
							}
							obj->tri_material[obj->num_triangles] = curr_mtl;
							obj->num_triangles++;
						}
						else
						{
							// 解析失敗
						}
					}
				}
			}

			// ポリゴン数が確保した配列の大きさを超えたら強制終了
			if (obj->num_triangles >= MAX_TRIANGLE_SIZE)
			{
				break;
			}
		}
	};

	// 必要な配列を確保しなおす
	Vector* new_array;
	int* new_nums;
	Mtl** new_mtls;

	new_array = new Vector[obj->num_vertices];
	memcpy(new_array, obj->vertices, sizeof(Vector)* obj->num_vertices);
	delete[] obj->vertices;
	obj->vertices = new_array;

	new_array = new Vector[obj->num_normals];
	memcpy(new_array, obj->normals, sizeof(Vector)* obj->num_normals);
	delete[] obj->normals;
	obj->normals = new_array;

	new_array = new Vector[obj->num_tex_coords];
	memcpy(new_array, obj->tex_coords, sizeof(Vector)* obj->num_tex_coords);
	delete[] obj->tex_coords;
	obj->tex_coords = new_array;

	new_nums = new int[obj->num_triangles * 3];
	memcpy(new_nums, obj->tri_v_no, sizeof(int)* obj-> num_triangles * 3);
	delete[] obj->tri_v_no;
	obj->tri_v_no = new_nums;
	
	new_nums = new int[obj->num_triangles * 3];
	memcpy(new_nums, obj->tri_vn_no, sizeof(int)* obj-> num_triangles * 3);
	delete[] obj->tri_vn_no;
	obj->tri_vn_no = new_nums;
	
	new_nums = new int[obj->num_triangles * 3];
	memcpy(new_nums, obj->tri_vt_no, sizeof(int)* obj->num_triangles * 3);
	delete[] obj->tri_vt_no;
	obj->tri_vt_no = new_nums;

	new_mtls = new Mtl * [obj->num_triangles];
	memcpy(new_mtls, obj->tri_material, sizeof(Mtl*)* obj->num_triangles);
	delete[] obj->tri_material;
	obj->tri_material = new_mtls;

	// ファイルを閉じる
	fclose(fp);

	// 読み込んだオブジェクトデータを返す
	return obj;
}

//
// Mtlファイルの読み込み
//
void LoadMtl(const char* filename, Obj* obj)
{
	FILE* fp;
	char line[BUFFER_LENGTH];
	Color color;
	Mtl* curr_mtl = NULL;

	// ファイルを開く
	errno_t err = fopen_s(&fp, filename, "r");
	if (err != 0 || fp == NULL)
	{
		return;
	}

	// Mtl配列を初期化
	obj->num_materials = 0;
	obj->materials = new Mtl * [MAX_MTL_SIZE];

	// ファイルから1行ずつ読み込み
	while (fgets(line, BUFFER_LENGTH, fp) != NULL)
	{
		// マテリアルデータの追加
		if (strncmp(line, "newmtl", 6) == 0)
		{
			char name[256] = "";

			// テキストを解析
			sscanf_s(line, "newmtl %s", name, (unsigned)_countof(name));

			if (strlen(name) == 0)
			{
				continue;
			}

			// マテリアルデータの作成
			curr_mtl = new Mtl();
			curr_mtl->name = new char[strlen(name) + 1];
			strcpy_s(curr_mtl->name, strlen(name) + 1, name);
			curr_mtl->kd.r = 0.8f;
			curr_mtl->kd.g = 0.8f;
			curr_mtl->kd.b = 0.8f;

			// マテリアルデータを配列に記録
			obj->materials[obj->num_materials] = curr_mtl;
			obj->num_materials++;
		}

		// 反射特性データの読み込み
		if (line[0] == 'K')
		{
			// 拡散反射特性（Kd）
			if (line[1] == 'd')
			{
				// テキストを解析
				sscanf_s(line, "Kd %f %f %f", &color.r, &color.g, &color.b);

				// 拡散反射特性（Kd）を記録
				if (curr_mtl)
				{
					curr_mtl->kd = color;
				}
			}
		}
	}

	// ファイルを閉じる
	fclose(fp);
}

//
// オブジェクトのスケーリング
//
float ScaleObj(Obj* obj, float max_size)
{
	if (!obj || (obj->vertices == 0))
	{
		return 0.0;
	}

	// サイズ計算
	Vector min, max;
	min = max = obj->vertices[0];
	int i;
	for (i = 0; i < obj->num_vertices; i++)
	{
		const Vector& p = obj->vertices[i];
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.z = p.z;
		if (p.x > max.x) max.x = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.z = p.z;
	}

	// スケーリング
	float size, scale;
	size = ((max.x - min.x) > (max.z - min.z)) ? (max.x - min.x) : (max.z - min.z);
	scale = max_size / size;
	for (i = 0; i < obj->num_vertices; i++)
	{
		obj->vertices[i].x *= scale;
		obj->vertices[i].y *= scale;
		obj->vertices[i].z *= scale;
	}

	// スケーリング後の中心の高さを返す
	float object_y = min.y * scale;
	return object_y;
}

//
// Obj形式データの描画
//
void RenderObj(Obj* obj)
{
	int i, j, no;
	Mtl* curr_mtl = NULL;

	glBegin(GL_TRIANGLES);
	for (i = 0; i < obj->num_triangles; i++)
	{
		// マテリアルの切り替え
		if ((obj->num_materials > 0) && (obj->tri_material[i] != curr_mtl))
		{
			curr_mtl = obj->tri_material[i];
			glColor3f(curr_mtl->kd.r, curr_mtl->kd.g, curr_mtl->kd.b);
		}

		// 三角面の各頂点データの指定
		for (j = 0; j < 3; j++)
		{
			// 法線ベクトル
			no = obj->tri_vn_no[i * 3 + j];
			const Vector& vn = obj->normals[no];
			glNormal3f(vn.x, vn.y, vn.z);

			// 頂点座標
			no = obj->tri_v_no[i * 3 + j];
			const Vector& v = obj->vertices[no];
			glVertex3f(v.x, v.y, v.z);
		}
	}

	glEnd();
}
