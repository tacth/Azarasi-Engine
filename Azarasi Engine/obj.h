#ifndef _OBJ_H_
#define _OBJ_H_

// �x�N�g���f�[�^
struct Vector
{
	float x, y, z;
};

// �J���[�f�[�^
struct Color
{
	float r, g, b;
};

// �}�e���A���f�[�^�iMtl�`���p�j
struct Mtl
{
	char* name; // �}�e���A����

	Color kd; // �g�U���ˌ�

	char* texture_name; // �e�N�X�`���摜�̃t�@�C����
};

// �􉽌`��f�[�^�iObj�`���p�j
struct Obj
{
	int num_vertices; // ���_��
	Vector* vertices; // ���_���W�z��

	int num_normals;
	Vector* normals; // �@���x�N�g���z��

	int num_tex_coords;
	Vector* tex_coords; // �e�N�X�`�����W�z��

	int num_triangles; // �O�p�ʐ�
	int* tri_v_no;     // �O�p�ʂ̊e���_�̒��_���W�ԍ��z��
	int* tri_vn_no;    // �O�p�ʂ̊e���_�̖@���x�N�g���ԍ��z��
	int* tri_vt_no;    // �O�p�ʂ̊e���_�̃e�N�X�`�����W�ԍ��z��

	Mtl** tri_material; // �O�p�ʂ̑f��

	int num_materials; // �}�e���A����
	Mtl** materials; // �}�e���A���̔z��
};

// Obj�t�@�C���̓ǂݍ���
Obj* LoadObj(const char* filename);

// Mtl�t�@�C���̓ǂݍ���
void LoadMtl(const char* filename, Obj* obj);

// �I�u�W�F�N�g�̃X�P�[�����O
float ScaleObj(Obj* obj, float max_size);

// Obj�`��f�[�^�̕`��
void RenderObj(Obj* obj);

#endif // _OBJ_H_
