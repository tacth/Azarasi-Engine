#include <Windows.h>
#include <stdio.h>
#include<math.h>
#include <GL/glut.h>

#include "obj.h"

// �O���[�o���ϐ�

// �E�B���h�E�̃T�C�Y
int win_width, win_height;

// �}�E�X�̃h���b�O�̂��߂̕ϐ�
int drag_mouse_r = 0; // �E�{�^�����h���b�O�����ǂ����̃t���O
                      // �i0:��h���b�O���A1:�h���b�O���j
int drag_mouse_l = 0; // ���{�^�����h���b�O�����ǂ����̃t���O
                      // �i0:��h���b�O���A1:�h���b�O���j
int last_mouse_x, last_mouse_y; // �Ō�ɋL�^���ꂽ�}�E�X�J�[�\���̍��W

// ���_����p�����^
float view_center_x; // �����_�̈ʒu
float view_center_y; // �����_�̈ʒu
float view_center_z; // �����_�̈ʒu
float view_yaw; // ���_�̕��ʊp
float view_pitch; // ���_�̋p
float view_distance; // ���_�ƒ����_�̋���

// ���_���샂�[�h
enum ViewControlModeEnum
{
    VIEW_DOLLY_PARAM, // Dolly���[�h�i�}��ϐ��j
    // VIEW_DOLLY_DIRECT, // Dolly���[�h�i���ڍX�V�j
    VIEW_SCROLL_PARAM, // Scroll���[�h�i�}��ϐ��j
    // VIEW_SCROLL_DIRECT, // Scroll���[�h�i���ڍX�V�j
    VIEW_WALKTHROUGH_PARAM, // Walkthrough���[�h�i�}��ϐ��j
    // VIEW_WALKTHROUGH_DIRECT, // Walkthrough���[�h�i���ڍX�V�j
    NUM_VIEW_CONTROL_MODES // ���_���샂�[�h�̎�ސ�
};

// ���_���샂�[�h�̖��O
const char* mode_name[] = {
    "Dolly Mode ( Parameter )",
    "Scroll Mode ( Parameter )",
    "Walkthrough Mode ( Parameter )",
};

// ���݂̎��_���샂�[�h
ViewControlModeEnum mode = VIEW_DOLLY_PARAM;

// �􉽌`��I�u�W�F�N�g
struct Obj* obj = NULL;
float object_y = 0.0f;

//
// ���_����̂��߂̏���
// 

//
// ���_�̏�����
//
void InitView()
{
    // ���_�p�����^��������
    if (mode == VIEW_DOLLY_PARAM)
    {
        view_center_x = 0.0f;
        view_center_y = 0.0f;
        view_center_z = 0.0f;
        view_yaw = -30.0f;
        view_pitch = -30.0f;
        view_distance = 15.0f;
    }
    if (mode == VIEW_SCROLL_PARAM)
    {
        view_center_x = 0.0f;
        view_center_y = 0.0f;
        view_center_z = 0.0f;
        view_yaw = 0.0f;
        view_pitch = -30.0f;
        view_distance = 15.0f;
    }
    if (mode == VIEW_WALKTHROUGH_PARAM)
    {
        view_center_x = 0.0f;
        view_center_y = 0.5f;
        view_center_z = 0.0f;
        view_yaw = 0.0f;
        view_pitch = 0.0f;
        view_distance = 0.0f;
    }

    // �ϊ��s���������
    //if (mode == VIEW_DOLLY_DIRECT)
    //{
    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadIdentity();
    //    glTranslatef(0.0, 0.0, -15.0);
    //    glRotatef(30.0, 1.0, 0.0, 0.0);
    //    glRotatef(30.0, 0.0, 1.0, 0.0);

    //    view_center_x = 0.0f;
    //    view_center_y = 0.0f;
    //    view_center_z = 0.0f;
    //}
    //if (mode == VIEW_SCROLL_DIRECT)
    //{
    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadIdentity();
    //    glTranslatef(0.0, 0.0, -15.0);
    //    glRotatef(30.0, 1.0, 0.0, 0.0);
    //    glRotatef(0.0, 0.0, 1.0, 0.0);

    //    view_center_x = 0.0f;
    //    view_center_y = 0.0f;
    //    view_center_z = 0.0f;
    //}
    //if (mode == VIEW_WALKTHROUGH_DIRECT)
    //{
    //    glMatrixMode(GL_MODELVIEW);
    //    glLoadIdentity();
    //    glTranslatef(0.0, -0.5, 0.0);
    //    glRotatef(0.0, 1.0, 0.0, 0.0);
    //    glRotatef(0.0, 0.0, 1.0, 0.0);
    //}
}

//
// ���_�p�����^�ɉ����ĕϊ��s����X�V
//
void UpdateViewMatrix()
{
    // ���_�p�����^���g�������쎞�̂ݕϊ��s����X�V
    if ((mode == VIEW_DOLLY_PARAM) || (mode == VIEW_SCROLL_PARAM) || (mode == VIEW_WALKTHROUGH_PARAM))
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -view_distance);
        glRotatef(-view_pitch, 1.0f, 0.0f, 0.0f);
        glRotatef(-view_yaw, 0.0f, 1.0f, 0.0f);
        glTranslatef(-view_center_x, -view_center_y, -view_center_z);
    }
}

//
// �}�E�X����ɉ����Ď��_�p�����^ or �ϊ��s����X�V
//
void UpdateView(int delta_mouse_right_x, int delta_mouse_right_y,
    int delta_mouse_left_x, int delta_mouse_left_y)
{
	// ���_�p�����^���X�V�iDolly���[�h�E�}��ϐ��j
    if (mode == VIEW_DOLLY_PARAM)
    {
        // �������̉E�{�^���h���b�O�ɉ����āA���_�𐅕������ɉ�]
        if (delta_mouse_right_x != 0)
        {
            view_yaw -= delta_mouse_right_x * 1.0f;

            // �p�����^�̒l������͈̔͂𒴂��Ȃ��悤�ɏC��
            if (view_yaw < 0)
            {
                view_yaw += 360;
            }
            else if (view_yaw > 360)
            {
                view_yaw -= 360;
            }
        }

        // �c�����̉E�{�^���h���b�O�ɉ����āA���_���㉺�����ɉ�]
        if (delta_mouse_right_y != 0)
        {
            view_pitch -= delta_mouse_right_y * 1.0f;

            // �p�����^�̒l������͈̔͂𒴂��Ȃ��悤�ɏC��
            if (view_pitch < -90)
            {
                view_pitch = -90;
            }
            else if (view_pitch > -2)
            {
                view_pitch = -2;
            }
        }

		// �c�����̍��{�^���h���b�O�ɉ����āA���_�ƒ����_�̋�����ύX
        if (delta_mouse_left_y != 0)
        {
            view_distance += delta_mouse_left_y * 0.2f;

			// �p�����^�̒l������͈̔͂𒴂��Ȃ��悤�ɏC��
            if (view_distance < 5)
            {
				view_distance = 5;
            }
        }
    }

    // ���_�p�����^���X�V�iScroll���[�h�E�}��ϐ��j
    if (mode == VIEW_SCROLL_PARAM)
    {
        // �c�����̉E�{�^���h���b�O�ɉ����āA���_���㉺�����ɉ�]
        if (delta_mouse_right_y != 0)
        {
            view_pitch -= delta_mouse_right_y * 1.0f;

            // �p�����^�̒l������͈̔͂𒴂��Ȃ��悤�ɏC��
            if (view_pitch < -90)
            {
                view_pitch = -90;
            }
            else if (view_pitch > -2)
            {
                view_pitch = -2;
            }
        }

        // ���{�^���h���b�O�ɉ����āA���_��O�㍶�E�Ɉړ�
        // �i���[���h���W����Ƃ����O�㍶�E�j
        if ((delta_mouse_left_x != 0) || delta_mouse_left_y != 0)
        {
            view_center_x += -delta_mouse_left_x * 0.1f;
            view_center_z += -delta_mouse_left_y * 0.1f;
        }
    }

    // ���_�p�����^���X�V�iWalkthrough���[�h�E�}��ϐ��j
    if (mode == VIEW_WALKTHROUGH_PARAM)
    {
        // �������̉E�{�^���h���b�O�ɉ����āA���_�𐅕������ɉ�]
        if (delta_mouse_right_x != 0)
        {
            view_yaw -= delta_mouse_right_x * 1.0f;

            // �p�����^�̒l������͈̔͂𒴂��Ȃ��悤�ɏC��
            if (view_yaw < 0)
            {
                view_yaw += 360;
            }
            else if (view_yaw > 360)
            {
                view_yaw -= 360;
            }
        }

        // ���{�^���h���b�O�ɉ����āA���_��O�㍶�E�Ɉړ�
        // �i�J�����̌�������Ƃ����O�㍶�E�j
        if ((delta_mouse_left_x != 0) || (delta_mouse_left_y != 0))
        {
            // ���E�̈ړ��ʁA�O��̈ړ��ʂ��v�Z
            float dz, dx;
            dz = delta_mouse_left_x * 0.1f;
            dx = delta_mouse_left_y * 0.1f;

            // ���݂̕ϊ��s��i�J�����̌����j���擾
            float m[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, m);

            // ���[���h���W�n�ł̃J�����̈ړ��ʂ��v�Z�i�O������j
            view_center_x += m[2] * dx;
            view_center_z += m[10] * dx;

            // ���[���h���W�n�ł̃J�����̈ړ��ʂ��v�Z�i���E�����j
            view_center_x += m[0] * dz;
            view_center_z += m[8] * dz;
        }
    }
}

//
// ���C������
//

//
// �i�q�͗l�̏���`��
//
void DrawFloor(int tile_size, int num_x, int num_z, float r0, float g0,
    float b0, float r1, float g1, float b1)
{
    int x, z;
    float ox, oz;

    glBegin(GL_QUADS);
    glNormal3d(0.0, 1.0, 0.0);

    ox = -(static_cast<float>(num_x) * tile_size) / 2;
    for (x = 0; x < num_x; x++)
    {
        oz = -(static_cast<float>(num_z) * tile_size) / 2;
        for (z = 0; z < num_z; z++)
        {
            if (((x + z) % 2) == 0)
            {
                glColor3f(r0, g0, b0);
            }
            else
            {
                glColor3f(r1, g1, b1);
            }

            glTexCoord2d(0.0f, 0.0f);
            glVertex3d(ox, 0.0, oz);
            glTexCoord2d(0.0f, 1.0f);
            glVertex3d(ox, 0.0, oz + tile_size);
            glTexCoord2d(1.0f, 1.0f);
            glVertex3d(ox + tile_size, 0.0, oz + tile_size);
            glTexCoord2d(1.0f, 0.0f);
            glVertex3d(ox + tile_size, 0.0, oz);

            oz += tile_size;
        }
        ox += tile_size;
    }
    glEnd();
}

//
// �������i���݂̃��[�h���j��`��
//
void DrawTextInformation()
{
    // �\�����郁�b�Z�[�W
    int i;
    const char* message = mode_name[mode];

    // �ˉe�s���������
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, win_width, win_height, 0.0);

    // ���f���r���[�s���������
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Z�o�b�t�@�E���C�e�B���O�𖳌��ɂ���
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // ���b�Z�[�W�̕`��
    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos2i(16, 16 + 18);
    for (i = 0; message[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
    }

    // �ݒ��S�ĕ���
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

//
// ��ʕ`�掞�ɌĂ΂��R�[���o�b�N�֐�
//
void DisplayCallback()
{
    // ��ʂ��N���A
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ���_�p�����^�ɉ����ĕϊ��s����X�V
    UpdateViewMatrix();

    // �����ʒu��ݒ�
    float light0_position[] = { 10.0, 10.0, 10.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    // �i�q�͗l�̏���`��
    DrawFloor(1, 10, 10, 1.0, 1.0, 1.0, 0.8f, 0.8f, 0.8f);

    // �􉽌`���`��
    if (obj)
    {
        glColor3f(1.0, 1.0, 1.0);

        // �ϊ��s���ݒ�
        glTranslatef(0.0f, -object_y, 0.0f);

        // �􉽌`���`��
        RenderObj(obj);
    }

    // �������i���݂̃��[�h���j��`��
    DrawTextInformation();

    // �o�b�N�o�b�t�@�ɕ`�悵����ʂ��t�����g�o�b�t�@�ɕ\��
    glutSwapBuffers();
}

//
// �E�B���h�E�T�C�Y�ύX���ɌĂ΂��R�[���o�b�N�֐�
//
void ReshapeCallback(int w, int h)
{
    // �E�B���h�E���̕`����s���͈͂�ݒ�
    glViewport(0, 0, w, h);

    // �J�������W�n����X�N���[�����W�n�ւ̕ϊ��s���ݒ�
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (double)w / h, 1, 500);

    // �E�B���h�E�T�C�Y�̋L�^
    win_width = w;
    win_height = h;
}

//
// �}�E�X�N���b�N���ɌĂ΂��R�[���o�b�N�֐�
//
void MouseClickCallback(int button, int state, int mx, int my)
{
    // �E�{�^���������ꂽ��h���b�O�J�n
    if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
    {
        drag_mouse_r = 1;
    }
    // �E�{�^���������ꂽ��h���b�O�I��
    else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
    {
        drag_mouse_r = 0;
    }

    // ���{�^���������ꂽ��h���b�O�J�n
    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
    {
        drag_mouse_l = 1;
    }
    // ���{�^���������ꂽ��h���b�O�I��
    else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
    {
        drag_mouse_l = 0;
    }

    // ���݂̃}�E�X���W���L�^
    last_mouse_x = mx;
    last_mouse_y = my;
}

//
// �}�E�X�h���b�O���ɌĂ΂��R�[���o�b�N�֐�
//
void MouseDragCallback(int mx, int my)
{
    // �}�E�X�̃h���b�O�������v�Z
    int delta_mouse_right_x = 0,
        delta_mouse_right_y = 0,
        delta_mouse_left_x = 0,
        delta_mouse_left_y = 0;

    if (drag_mouse_r)
    {
        delta_mouse_right_x = mx - last_mouse_x;
        delta_mouse_right_y = my - last_mouse_y;
    }
    if (drag_mouse_l)
    {
        delta_mouse_left_x = mx - last_mouse_x;
        delta_mouse_left_y = my - last_mouse_y;
    }

    // �}�E�X����ɉ����Ď��_�p�����^ or �ϊ��s����X�V
    UpdateView(delta_mouse_right_x, delta_mouse_right_y,
        delta_mouse_left_x, delta_mouse_left_y);

    // ����̃}�E�X���W���L�^
    last_mouse_x = mx;
    last_mouse_y = my;

    // �ĕ`��̎w�����o��
    glutPostRedisplay();
}

//
// �L�[�{�[�h�̃L�[�������ꂽ�Ƃ��ɌĂ΂��R�[���o�b�N�֐�
//
void KeyboardCallback(unsigned char key, int mx, int my)
{
    // M�L�[�Ŏ��_���샂�[�h�����Ԃɐ؂�ւ�
    if (key == 'm')
    {
        // ���̎��_���샂�[�h�ɐ؂�ւ�
        mode = (ViewControlModeEnum)((mode + 1) % NUM_VIEW_CONTROL_MODES);

        // ���_�̏�����
        InitView();

        // �ĕ`��̎w�����o��
        glutPostRedisplay();
    }

    // L�L�[�Ŋ􉽌`���ǂݍ���
    if (key == 'l')
    {
        const int file_name_len = 256;
        char file_name[file_name_len] = "";

        // �t�@�C���_�C�A���O�̐ݒ�
        OPENFILENAME open_file;
        memset(&open_file, 0, sizeof(OPENFILENAME));
        open_file.lStructSize = sizeof(OPENFILENAME);
        open_file.hwndOwner = NULL;
        open_file.lpstrFilter = TEXT("Obj File (*.obj)\0*.obj\0All File (*.*)\0*.*\0");
        open_file.nFilterIndex = 1;
        open_file.lpstrFile = file_name;
        open_file.nMaxFile = file_name_len;
        open_file.lpstrTitle = TEXT("Obj File");
        open_file.lpstrDefExt = TEXT("obj");
        open_file.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

        // �t�@�C���_�C�����O��\��
        BOOL ret = GetOpenFileName(&open_file);

        // �t�@�C�����w�肳�ꂽ��V�����􉽌`���ǂݍ���
        if (ret)
        {
            // �􉽌`��t�@�C���̓ǂݍ���
            obj = LoadObj(file_name);

            // �ǂݍ��񂾊􉽌`����X�P�[�����O����
            object_y = ScaleObj(obj, 5.0f);

            // �ĕ`��̎w�����o��
            glutPostRedisplay();
        }
    }
}

//
// ���������֐�
//
void InitEnvironment()
{
    // �������쐬����
    float light0_position[] = { 10.0f, 10.0f, 10.0f, 1.0f };
    float light0_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float light0_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glEnable(GL_LIGHT0);

    // �����v�Z��L���ɂ���
    glEnable(GL_LIGHTING);

    // ���̂̐F����L���ɂ���
    glEnable(GL_COLOR_MATERIAL);

    // Z�e�X�g��L���ɂ���
    glEnable(GL_DEPTH_TEST);

    // �w�ʏ�����L���ɂ���
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // �w�i�F��ݒ�
    glClearColor(0.5f, 0.5f, 0.8f, 0.0f);
}

//
// ���C���֐�
//
int main(int argc, char** argv)
{
    // GLUT�̏�����
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Azarasi Engine");

    // �R�[���o�b�N�֐��̓o�^
    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutMouseFunc(MouseClickCallback);
    glutMotionFunc(MouseDragCallback);
    glutKeyboardFunc(KeyboardCallback);

    // ��������
    InitEnvironment();

    // ���_�̏�����
    InitView();

    // GLUT�̃��C�����[�v�ɏ������ڂ�
    glutMainLoop();
    return 0;
}
