#include "GLMetaseq.h"
#include <math.h>
#include <stdarg.h>
#define _CRT_SECURE_NO_WARNINGS 1
//math.hのM_PIが使えないので、円周率を独自に定義
#define PI 3.14159265358979323846

#define	IMAGEWIDTH 256
#define	IMAGEHEIGHT 256

#define SUN_ELEVATION 60.0
#define SUN_DIRECTION 115.0

#define MAP_SIZE_X 20000.0
#define MAP_SIZE_Z 20000.0


//MQOモデル変数
MQO_MODEL model;
unsigned char mouseFlag = GL_FALSE;
int xStart, yStart;
double xAngle = 0.0, yAngle = 0.0;

//飛行機の速度を記録する変数
double plane_speed = 1.0;
//飛行機の角度を調整する変数
double plane_angle_yaw = 0.0, plane_angle_pitch = 0.0, plane_angle_role = 0.0;
//飛行機の座標を記録する変数
double plane_x = 0, plane_y = 1000.0, plane_z = 0;
//ヨーイング・ピッチングの量を記録する変数
double diff_yaw = 0.0, diff_pitch = 0.0;
//カメラの角度を調整するための変数
double camera_angle_x = 0, camera_angle_y = 0, camera_angle_z = 0;
//平行光源の角度を決めるための変数
double sun_x, sun_y, sun_z, nomarize;

unsigned char texImage[IMAGEHEIGHT][IMAGEWIDTH][3];
GLfloat sea_color[] = { 0.7, 0.7, 1.0, 1.0 };

char mode_flag = 0;

void mySetLight(void)
{
	//Z軸のマイナス方向を北としたとき、7月19日の10:00ぐらいの太陽を想定
	sun_x = sin(SUN_DIRECTION * PI / 180);
	sun_y = sin(SUN_ELEVATION * PI / 180);
	sun_z = -cos(SUN_DIRECTION * PI / 180);
	nomarize = sqrt(pow(sun_x, 2.0) + pow(sun_y, 2) + pow(sun_z, 2));

	GLfloat light0_diffuse[] = { 1.0f, 0.8f, 0.8f, 1.0 };	// 拡散反射光
	GLfloat light0_specular[] = { 0.8, 0.8, 0.8, 1.0 };	// 鏡面反射光
	GLfloat light0_ambient[] = { 0.7, 0.7, 0.7, 1.0 };	// 環境光
	GLfloat light0_position[] = {(float)sun_x / (float)nomarize ,(float)sun_y / (float)nomarize , (float)sun_z / (float)nomarize, 0.0 };	// 位置と種類

	GLfloat light1_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };	//拡散反射光
	GLfloat light1_specular[] = { 0.0, 0.0, 0.0, 1.0 };	//鏡面反射光
	GLfloat light1_ambient[] = { 0.5, 0.5, 0.5, 1.0 };	//環境光
	GLfloat light1_position[] = { 0.0, 1.0, 1.0, 0.0 };	//ライト1の位置
	
	//ライト0の設定
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);	 // 拡散反射光の設定
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular); // 鏡面反射光の設定
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);	 // 環境光の設定
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position); // 位置と種類の設定

	//ライト1の設定
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	
	glShadeModel(GL_SMOOTH);	// シェーディングの種類の設定
	if (mode_flag)
	{
		glEnable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
	}
	else
	{
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
	}
}

void myPrintf(int x, int y, char *aFmt, ...){
	int  i;
	char buf[1024];
	va_list ap;					// defined in <stdarg.h>

	glPushMatrix();
	glLoadIdentity();
	va_start(ap, aFmt);			// defined in <stdarg.h>
	vsprintf(buf, aFmt, ap);	// ditto
	glRasterPos3d(0, 0, -2);
	glBitmap(0, 0, 0, 0, (int)x, (int)y, NULL);  /* adjust position */
	for (i = 0; i < strlen(buf); i++){
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, buf[i]);
	}
	va_end(ap);
	glPopMatrix();
}

//画像読み込み
void readPPMImage(char* filename)
{
	FILE *fp;

	if ((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "Cannot open ppm file %s\n", filename);
		exit(1);
	}

	fread(texImage, 1, IMAGEWIDTH * IMAGEHEIGHT * 3, fp);	// read RGB data
	fclose(fp);
}

//テクスチャのセットアップ
void setUpTexture(char* filename)
{
	readPPMImage(filename);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGEWIDTH, IMAGEHEIGHT,
		0, GL_RGB, GL_UNSIGNED_BYTE, texImage);
}

void myInit(char *progname)
{
	int width = 640, height = 480;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 50);
	glutCreateWindow(progname);
	glClearColor(0.3f, 0.5f, 0.8f, 0.0f);
}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();
	if (mode_flag)
	{
		//カメラ位置の更新
		camera_angle_y = cos(plane_angle_pitch * PI / 180);
		camera_angle_z = sin(plane_angle_pitch * PI / 180) * cos(plane_angle_yaw * PI / 180);
		camera_angle_x = sin(plane_angle_pitch * PI / 180) * sin(plane_angle_yaw * PI / 180);

		//カメラ角度の更新
		gluLookAt(plane_x + sin(plane_angle_yaw * PI / 180) * cos(plane_angle_pitch * PI / 180) * 100.0, plane_y - sin(plane_angle_pitch * PI / 180) * 100.0, -plane_z + cos(plane_angle_yaw * PI / 180) * cos(plane_angle_pitch * PI / 180) * 100.0, plane_x, plane_y, -plane_z, camera_angle_x, camera_angle_y, camera_angle_z);
	}
	else
	{
		gluLookAt(0, 0, 100, 0, 0, 0, 0, 1, 0);
	}
	//ライティング
	mySetLight();
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	//海面の作成
	if (mode_flag)
	{
		setUpTexture("sea_surface.ppm");
		glPushMatrix();
		{
			//海面の法線ベクトル設定
			glNormal3d(0.0, 1.0, 0.0);
			//海面の描画
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sea_color);
			glBegin(GL_QUADS);
			glTexCoord2d(0.0, 0.0); glVertex3d(-1.0 * MAP_SIZE_X, 0.0, -1.0 * MAP_SIZE_Z);
			glTexCoord2d(0.0, 100.0); glVertex3d(-1.0 * MAP_SIZE_X, 0.0, MAP_SIZE_Z);
			glTexCoord2d(100.0, 100.0); glVertex3d(MAP_SIZE_X, 0.0, MAP_SIZE_Z);
			glTexCoord2d(100.0, .0); glVertex3d(MAP_SIZE_X, 0.0, -1.0 * MAP_SIZE_Z);
			glEnd();
		}
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		{
			glNormal3d(0.0, 1.0, 0.0);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, sea_color);
			glBegin(GL_QUADS);
			glVertex3d(-100.0, -200.0, -100.0);
			glVertex3d(-100.0, -200.0, 100.0);
			glVertex3d(100.0, -200.0, 100.0);
			glVertex3d(100.0, -200.0, -100.0);
			glEnd();
		}
		glPopMatrix();
		
	}
	glDisable(GL_TEXTURE_2D);

	if (mode_flag)
	{
		//飛行機の描画
		glPushMatrix();
		{
			//飛行機の移動処理
			glTranslated(plane_x, plane_y, -plane_z);
			//ヨーイング回転処理
			glRotated(plane_angle_yaw, 0.0, 1.0, 0.0);
			//ピッチ回転処理
			glRotated(plane_angle_pitch, 1.0, 0.0, 0.0);
			//ロール回転処理
			if (plane_angle_role > 30.0)
			{
				glRotated(30.0, 0.0, 0.0, 1.0);
			}
			else if (plane_angle_role < -30.0)
			{
				glRotated(-30.0, 0.0, 0.0, 1.0);
			}
			else
			{
				glRotated(plane_angle_role, 0.0, 0.0, 1.0);
			}
			//デフォルトではz軸のプラスの方向を向いているので、マイナスの方向に向ける
			glRotatef(180.0, 0.0, 1.0, 0.0);
			//モデル呼び出し
			mqoCallModel(model);
		}
		glPopMatrix();
	}
	else
	{
		glPushMatrix();
		{
			glRotated(yAngle, 0.0, 1.0, 0.0);
			glRotated(xAngle, 0.0, 0.0, 1.0);
			glRotated(90.0, 0.0, 1.0, 0.0);
			mqoCallModel(model);
		}
		glPopMatrix();
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	if (mode_flag)
	{
		glPushMatrix();
		glColor3d(0.0, 0.1, 0.1);
		myPrintf(-300, 200, "Altitude : %lf", plane_y);
		myPrintf(-300, 200 - 13 * 1, "Speed : %lf", plane_speed);
		myPrintf(-300, 200 - 13 * 2, "Angle : %lf", plane_angle_yaw);
		glPopMatrix();
	}

	glutSwapBuffers();
}

void myIdle(void)
{
	if (mode_flag)
	{
		//飛行機の角度を更新
		plane_angle_pitch = fmod(plane_angle_pitch + 360.0 + diff_pitch, 360);
		plane_angle_yaw = fmod(plane_angle_yaw + 360.0 + diff_yaw, 360.0);

		//飛行機の座標更新
		plane_z += cos(plane_angle_yaw * PI / 180) * cos(plane_angle_pitch * PI / 180) * plane_speed;
		plane_x -= sin(plane_angle_yaw * PI / 180) * cos(plane_angle_pitch * PI / 180) * plane_speed;
		plane_y += sin(plane_angle_pitch * PI / 180) * plane_speed;
		//ロール角度更新
		if (cos(plane_angle_pitch * PI / 180) < 0)
		{
			plane_angle_role -= diff_yaw;
		}
		else if (diff_yaw == 0.0)
		{
			//ロール復帰処理
			plane_angle_role -= plane_angle_role / (fabsf(plane_angle_role) + 30.0);
		}
		else
		{
			plane_angle_role += diff_yaw;
		}
		if (plane_angle_role > 40.0)
		{
			plane_angle_role = 40.0;
		}
		else if (plane_angle_role < -40.0)
		{
			plane_angle_role = -40.0;
		}
		
		diff_yaw = 0;
		diff_pitch = 0;

		//限界まで行ったらxz座標の初期化
		if (abs((int)plane_x) > 10000.0 || abs((int)plane_z) > 10000.0)
		{
			plane_x = 0;
			plane_z = 0;
			camera_angle_x = 0.0;
			camera_angle_y = 0.0;
			camera_angle_z = 0.0;
		}
		if (plane_y > 10000.0 || plane_y <= 0.0)
		{
			plane_y = 1000.0;
			plane_angle_pitch = 0.0;
		}
	}
	else
	{
		plane_x = 0.0;
		plane_y = 1000.0;
		plane_z = 0.0;
	}
	glutPostRedisplay();
}

void myReshape(int width, int height)
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (mode_flag)
	{
		gluPerspective(60.0, (double)width / (double)height, 1.0, 10000.0);
		gluLookAt(0.0, 50.0, 400.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	else
	{
		gluPerspective(60.0, (double)width / (double)height, 0.1, 1000.0);
		gluLookAt(0.0, 10.0, 200.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
}

void myMouseFunc(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON  && state == GLUT_DOWN){
		xStart = x;
		yStart = y;
		mouseFlag = GL_TRUE;
	}
	else {
		mouseFlag = GL_FALSE;
	}
}

void myMouseMotion(int x, int y)
{
	int		xdis, ydis;
	double	a = 0.5;

	if (mouseFlag == GL_FALSE) return;
	xdis = x - xStart;
	ydis = y - yStart;
	/* To match mouse's movement and its rotation axis */
	xAngle -= (double)ydis *a;
	yAngle += (double)xdis *a;

	xStart = x;
	yStart = y;
	glutPostRedisplay();
}


void mySpecialKey(int key, int x, int y)
{
	
	switch (key)
	{
	case GLUT_KEY_DOWN:
		diff_pitch = 2.5;
		break;
	case GLUT_KEY_UP:
		diff_pitch = -1.0;
		break;
	case GLUT_KEY_RIGHT:
		diff_yaw = -2.0;
		break;
	case GLUT_KEY_LEFT:
		diff_yaw = 2.0;
		break;
	}
	//飛行機の上下が反転したら、左右も反転する
	if (cos(plane_angle_pitch * PI / 180) < 0)
	{
		diff_yaw *= -1.0;
	}

	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		if (plane_speed >= 3.0)
		{
			break;
		}
		plane_speed += 0.1;
		break;
	case 's':
		if (plane_speed <= 1.0)
		{
			break;
		}
		plane_speed -= 0.1;
		break;
	case 27 :
		mqoCleanup();
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void getValueFromMenu(int value)
{
	switch (value){
	case 1:
		mode_flag = 1;
		myReshape(640, 480);
		break;
	case 2:
		mode_flag = 0;
		myReshape(640, 480);
		break;
	case 3:
		plane_angle_pitch = 0;
		plane_angle_role = 0;
		plane_angle_yaw = 0;
		plane_x = 0;
		plane_y = 0;
		plane_z = 0;
		plane_speed = 1.0;
		xAngle = 0.0;
		yAngle = 0.0;
		break;
	default:
		break;
	}
}

void mySetMenu()
{
	glutCreateMenu(getValueFromMenu);
	glutAddMenuEntry("フライトモード", 1);
	glutAddMenuEntry("鑑賞モード", 2);
	glutAddMenuEntry("座標・角度のリセット", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	myInit(argv[0]);
	mqoInit();
	model = mqoCreateModel("air_plane_mini.mqo", 1.0);
	if (model == NULL)
	{
		fprintf(stderr, "読み込みエラー\n");
		exit(1);
	}
	mySetMenu();
	mySetLight();
	glutIdleFunc(myIdle);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myMouseMotion);
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutSpecialFunc(mySpecialKey);
	glutKeyboardFunc(myKeyboard);
	glutMainLoop();
	mqoCleanup();
	return 0;
}