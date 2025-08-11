#include <Windows.h>
#include <stdio.h>
#include<math.h>
#include <GL/glut.h>

#include "obj.h"

// グローバル変数

// ウィンドウのサイズ
int win_width, win_height;

// マウスのドラッグのための変数
int drag_mouse_r = 0; // 右ボタンをドラッグ中かどうかのフラグ
                      // （0:非ドラッグ中、1:ドラッグ中）
int drag_mouse_l = 0; // 左ボタンをドラッグ中かどうかのフラグ
                      // （0:非ドラッグ中、1:ドラッグ中）
int last_mouse_x, last_mouse_y; // 最後に記録されたマウスカーソルの座標

// 視点操作パラメタ
float view_center_x; // 注視点の位置
float view_center_y; // 注視点の位置
float view_center_z; // 注視点の位置
float view_yaw; // 視点の方位角
float view_pitch; // 視点の仰角
float view_distance; // 視点と注視点の距離

// 視点操作モード
enum ViewControlModeEnum
{
    VIEW_DOLLY_PARAM, // Dollyモード（媒介変数）
    // VIEW_DOLLY_DIRECT, // Dollyモード（直接更新）
    VIEW_SCROLL_PARAM, // Scrollモード（媒介変数）
    // VIEW_SCROLL_DIRECT, // Scrollモード（直接更新）
    VIEW_WALKTHROUGH_PARAM, // Walkthroughモード（媒介変数）
    // VIEW_WALKTHROUGH_DIRECT, // Walkthroughモード（直接更新）
    NUM_VIEW_CONTROL_MODES // 視点操作モードの種類数
};

// 視点操作モードの名前
const char* mode_name[] = {
    "Dolly Mode ( Parameter )",
    "Scroll Mode ( Parameter )",
    "Walkthrough Mode ( Parameter )",
};

// 現在の視点操作モード
ViewControlModeEnum mode = VIEW_DOLLY_PARAM;

// 幾何形状オブジェクト
struct Obj* obj = NULL;
float object_y = 0.0f;

//
// 視点操作のための処理
// 

//
// 視点の初期化
//
void InitView()
{
    // 視点パラメタを初期化
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

    // 変換行列を初期化
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
// 視点パラメタに応じて変換行列を更新
//
void UpdateViewMatrix()
{
    // 視点パラメタを使った操作時のみ変換行列を更新
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
// マウス操作に応じて視点パラメタ or 変換行列を更新
//
void UpdateView(int delta_mouse_right_x, int delta_mouse_right_y,
    int delta_mouse_left_x, int delta_mouse_left_y)
{
	// 視点パラメタを更新（Dollyモード・媒介変数）
    if (mode == VIEW_DOLLY_PARAM)
    {
        // 横方向の右ボタンドラッグに応じて、視点を水平方向に回転
        if (delta_mouse_right_x != 0)
        {
            view_yaw -= delta_mouse_right_x * 1.0f;

            // パラメタの値が所定の範囲を超えないように修正
            if (view_yaw < 0)
            {
                view_yaw += 360;
            }
            else if (view_yaw > 360)
            {
                view_yaw -= 360;
            }
        }

        // 縦方向の右ボタンドラッグに応じて、視点を上下方向に回転
        if (delta_mouse_right_y != 0)
        {
            view_pitch -= delta_mouse_right_y * 1.0f;

            // パラメタの値が所定の範囲を超えないように修正
            if (view_pitch < -90)
            {
                view_pitch = -90;
            }
            else if (view_pitch > -2)
            {
                view_pitch = -2;
            }
        }

		// 縦方向の左ボタンドラッグに応じて、視点と注視点の距離を変更
        if (delta_mouse_left_y != 0)
        {
            view_distance += delta_mouse_left_y * 0.2f;

			// パラメタの値が所定の範囲を超えないように修正
            if (view_distance < 5)
            {
				view_distance = 5;
            }
        }
    }

    // 視点パラメタを更新（Scrollモード・媒介変数）
    if (mode == VIEW_SCROLL_PARAM)
    {
        // 縦方向の右ボタンドラッグに応じて、視点を上下方向に回転
        if (delta_mouse_right_y != 0)
        {
            view_pitch -= delta_mouse_right_y * 1.0f;

            // パラメタの値が所定の範囲を超えないように修正
            if (view_pitch < -90)
            {
                view_pitch = -90;
            }
            else if (view_pitch > -2)
            {
                view_pitch = -2;
            }
        }

        // 左ボタンドラッグに応じて、視点を前後左右に移動
        // （ワールド座標を基準とした前後左右）
        if ((delta_mouse_left_x != 0) || delta_mouse_left_y != 0)
        {
            view_center_x += -delta_mouse_left_x * 0.1f;
            view_center_z += -delta_mouse_left_y * 0.1f;
        }
    }

    // 視点パラメタを更新（Walkthroughモード・媒介変数）
    if (mode == VIEW_WALKTHROUGH_PARAM)
    {
        // 横方向の右ボタンドラッグに応じて、視点を水平方向に回転
        if (delta_mouse_right_x != 0)
        {
            view_yaw -= delta_mouse_right_x * 1.0f;

            // パラメタの値が所定の範囲を超えないように修正
            if (view_yaw < 0)
            {
                view_yaw += 360;
            }
            else if (view_yaw > 360)
            {
                view_yaw -= 360;
            }
        }

        // 左ボタンドラッグに応じて、視点を前後左右に移動
        // （カメラの向きを基準とした前後左右）
        if ((delta_mouse_left_x != 0) || (delta_mouse_left_y != 0))
        {
            // 左右の移動量、前後の移動量を計算
            float dz, dx;
            dz = delta_mouse_left_x * 0.1f;
            dx = delta_mouse_left_y * 0.1f;

            // 現在の変換行列（カメラの向き）を取得
            float m[16];
            glGetFloatv(GL_MODELVIEW_MATRIX, m);

            // ワールド座標系でのカメラの移動量を計算（前後方向）
            view_center_x += m[2] * dx;
            view_center_z += m[10] * dx;

            // ワールド座標系でのカメラの移動量を計算（左右方向）
            view_center_x += m[0] * dz;
            view_center_z += m[8] * dz;
        }
    }
}

//
// メイン処理
//

//
// 格子模様の床を描画
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
// 文字情報（現在のモード名）を描画
//
void DrawTextInformation()
{
    // 表示するメッセージ
    int i;
    const char* message = mode_name[mode];

    // 射影行列を初期化
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, win_width, win_height, 0.0);

    // モデルビュー行列を初期化
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Zバッファ・ライティングを無効にする
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // メッセージの描画
    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos2i(16, 16 + 18);
    for (i = 0; message[i] != '\0'; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message[i]);
    }

    // 設定を全て復元
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

//
// 画面描画時に呼ばれるコールバック関数
//
void DisplayCallback()
{
    // 画面をクリア
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 視点パラメタに応じて変換行列を更新
    UpdateViewMatrix();

    // 光源位置を設定
    float light0_position[] = { 10.0, 10.0, 10.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    // 格子模様の床を描画
    DrawFloor(1, 10, 10, 1.0, 1.0, 1.0, 0.8f, 0.8f, 0.8f);

    // 幾何形状を描画
    if (obj)
    {
        glColor3f(1.0, 1.0, 1.0);

        // 変換行列を設定
        glTranslatef(0.0f, -object_y, 0.0f);

        // 幾何形状を描画
        RenderObj(obj);
    }

    // 文字情報（現在のモード名）を描画
    DrawTextInformation();

    // バックバッファに描画した画面をフロントバッファに表示
    glutSwapBuffers();
}

//
// ウィンドウサイズ変更時に呼ばれるコールバック関数
//
void ReshapeCallback(int w, int h)
{
    // ウィンドウ内の描画を行う範囲を設定
    glViewport(0, 0, w, h);

    // カメラ座標系からスクリーン座標系への変換行列を設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (double)w / h, 1, 500);

    // ウィンドウサイズの記録
    win_width = w;
    win_height = h;
}

//
// マウスクリック時に呼ばれるコールバック関数
//
void MouseClickCallback(int button, int state, int mx, int my)
{
    // 右ボタンが押されたらドラッグ開始
    if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
    {
        drag_mouse_r = 1;
    }
    // 右ボタンが離されたらドラッグ終了
    else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
    {
        drag_mouse_r = 0;
    }

    // 左ボタンが押されたらドラッグ開始
    if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
    {
        drag_mouse_l = 1;
    }
    // 左ボタンが離されたらドラッグ終了
    else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
    {
        drag_mouse_l = 0;
    }

    // 現在のマウス座標を記録
    last_mouse_x = mx;
    last_mouse_y = my;
}

//
// マウスドラッグ時に呼ばれるコールバック関数
//
void MouseDragCallback(int mx, int my)
{
    // マウスのドラッグ距離を計算
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

    // マウス操作に応じて視点パラメタ or 変換行列を更新
    UpdateView(delta_mouse_right_x, delta_mouse_right_y,
        delta_mouse_left_x, delta_mouse_left_y);

    // 今回のマウス座標を記録
    last_mouse_x = mx;
    last_mouse_y = my;

    // 再描画の指示を出す
    glutPostRedisplay();
}

//
// キーボードのキーが押されたときに呼ばれるコールバック関数
//
void KeyboardCallback(unsigned char key, int mx, int my)
{
    // Mキーで視点操作モードを順番に切り替え
    if (key == 'm')
    {
        // 次の視点操作モードに切り替え
        mode = (ViewControlModeEnum)((mode + 1) % NUM_VIEW_CONTROL_MODES);

        // 視点の初期化
        InitView();

        // 再描画の指示を出す
        glutPostRedisplay();
    }

    // Lキーで幾何形状を読み込み
    if (key == 'l')
    {
        const int file_name_len = 256;
        char file_name[file_name_len] = "";

        // ファイルダイアログの設定
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

        // ファイルダイヤログを表示
        BOOL ret = GetOpenFileName(&open_file);

        // ファイルが指定されたら新しい幾何形状を読み込み
        if (ret)
        {
            // 幾何形状ファイルの読み込み
            obj = LoadObj(file_name);

            // 読み込んだ幾何形状をスケーリングする
            object_y = ScaleObj(obj, 5.0f);

            // 再描画の指示を出す
            glutPostRedisplay();
        }
    }
}

//
// 環境初期化関数
//
void InitEnvironment()
{
    // 光源を作成する
    float light0_position[] = { 10.0f, 10.0f, 10.0f, 1.0f };
    float light0_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    float light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float light0_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glEnable(GL_LIGHT0);

    // 光源計算を有効にする
    glEnable(GL_LIGHTING);

    // 物体の色情報を有効にする
    glEnable(GL_COLOR_MATERIAL);

    // Zテストを有効にする
    glEnable(GL_DEPTH_TEST);

    // 背面除去を有効にする
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    // 背景色を設定
    glClearColor(0.5f, 0.5f, 0.8f, 0.0f);
}

//
// メイン関数
//
int main(int argc, char** argv)
{
    // GLUTの初期化
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Azarasi Engine");

    // コールバック関数の登録
    glutDisplayFunc(DisplayCallback);
    glutReshapeFunc(ReshapeCallback);
    glutMouseFunc(MouseClickCallback);
    glutMotionFunc(MouseDragCallback);
    glutKeyboardFunc(KeyboardCallback);

    // 環境初期化
    InitEnvironment();

    // 視点の初期化
    InitView();

    // GLUTのメインループに処理を移す
    glutMainLoop();
    return 0;
}
