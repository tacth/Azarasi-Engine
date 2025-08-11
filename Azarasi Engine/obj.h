#ifndef _OBJ_H_
#define _OBJ_H_

// ベクトルデータ
struct Vector
{
	float x, y, z;
};

// カラーデータ
struct Color
{
	float r, g, b;
};

// マテリアルデータ（Mtl形式用）
struct Mtl
{
	char* name; // マテリアル名

	Color kd; // 拡散反射光

	char* texture_name; // テクスチャ画像のファイル名
};

// 幾何形状データ（Obj形式用）
struct Obj
{
	int num_vertices; // 頂点数
	Vector* vertices; // 頂点座標配列

	int num_normals;
	Vector* normals; // 法線ベクトル配列

	int num_tex_coords;
	Vector* tex_coords; // テクスチャ座標配列

	int num_triangles; // 三角面数
	int* tri_v_no;     // 三角面の各頂点の頂点座標番号配列
	int* tri_vn_no;    // 三角面の各頂点の法線ベクトル番号配列
	int* tri_vt_no;    // 三角面の各頂点のテクスチャ座標番号配列

	Mtl** tri_material; // 三角面の素材

	int num_materials; // マテリアル数
	Mtl** materials; // マテリアルの配列
};

// Objファイルの読み込み
Obj* LoadObj(const char* filename);

// Mtlファイルの読み込み
void LoadMtl(const char* filename, Obj* obj);

// オブジェクトのスケーリング
float ScaleObj(Obj* obj, float max_size);

// Obj形状データの描画
void RenderObj(Obj* obj);

#endif // _OBJ_H_
