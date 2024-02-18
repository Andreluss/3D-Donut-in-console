//Author: Andrzej Jabłoński
//Small, but math-heavy project that render a spinning 3D donut in console
//By default it's meant to run on windows, 
//but for linux the only difference is the system("cls") or system("clear") command at line 159
#include "bits/stdc++.h"
using namespace std;
using LL = long long;
#define SIZE(x) (int)(x).size()
#define FOR(i, b, e) for(auto i = (b); i <= (e); i++)
#define DFOR(i, b, e) for(auto i = (b); i >= (e); i--)
#define P2(x) (1LL<<x)
#define st first
#define nd second
#define mt make_tuple
#define SONIC_SPEED() ios::sync_with_stdio(0); cin.tie(0);

#define DEBUG 1
#define debug if(DEBUG)

struct Vector3
{
	float x, y, z;
	Vector3(float x = 0, float y = 0, float z = 0) :x(x), y(y), z(z) { }
	Vector3 normalize() const {
		float magn = sqrt(x * x + y * y + z * z);
		return Vector3(x / magn, y / magn, z / magn);//chyba
	}
};
float dot(const Vector3& lhs, const Vector3& rhs)
{
	return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

struct Matrix3D
{
	vector<vector<float>> M;
	Matrix3D() : M(3, vector<float>(3)) {}
	Matrix3D(const vector<vector<float>>& matrix)
	{
		M = matrix;
	}
	float& operator() (int i, int j) {
		return M[i][j];
	}
	float operator() (int i, int j) const {
		return M[i][j];
	}
};
Vector3 operator* (const Vector3& v, const Matrix3D& m)
{
	return Vector3(v.x * m(0, 0) + v.y * m(1, 0) + v.z * m(2, 0),
		v.x * m(0, 1) + v.y * m(1, 1) + v.z * m(2, 1),
		v.x * m(0, 2) + v.y * m(1, 2) + v.z * m(2, 2));
}
Matrix3D operator* (const Matrix3D& A, const Matrix3D& B)
{
	Matrix3D C;//zero wszedzie
	FOR(i, 0, 2)
		FOR(k, 0, 2)
		FOR(j, 0, 2)
		C(i, j) += A(i, k) * B(k, j);
	return C;
}



int main()
{
	//ios::sync_with_stdio(0);// cin.tie(0);
	const float PI = 3.1415f;

	const int H = 28, W = 100;//height, width of buffer window
	const float R1 = 1.25f, R2 = 2.3;
	const float K1 = 18, K2 = 6;//? K1

	Vector3 Light(0, -0.9, 1);
	Light = Light.normalize();
	const float A = 18, B = 5;//katy obrotu (12,5), (18, 5) (18,6) (20, 9)
	float a = 0, b = 0;

	for (int k = 0; ; k++, a += A, b += B) {
		if (a > 360) a -= 360;
		if (b > 360) b -= 360;

		vector<vector<char>> color(W, vector<char>(H, ' '));//[x][y]
		vector<vector<float>> zbuff(W, vector<float>(H, FLT_MAX));

		float tetha_step = 3.6f,
			phi_step = 1.0f;
		for (float tetha = 0; tetha < 360; tetha += tetha_step) {

			for (float phi = 0; phi < 360; phi += phi_step) {
				//pos obracamy wokol OY o (phi) stopni
				float cos_phi = cos(phi * PI / 180.0f);
				float sin_phi = sin(phi * PI / 180.0f);
				float cos_a = cos(a * PI / 180.0f);
				float sin_a = sin(a * PI / 180.0f);
				float cos_b = cos(b * PI / 180.0f);
				float sin_b = sin(b * PI / 180.0f);


				Matrix3D torus_rot({ {cos_phi,    0,   sin_phi},
									{   0,       1,      0   },
									{-sin_phi,   0,   cos_phi} });

				//i jeszcze o k*A wokol OZ
				Matrix3D A_rot({ {    1,      0,       0   },
									{    0,    cos_a,   sin_a },
									{    0,   -sin_a,   cos_a } });

				//i jeszcze o k*B wokol OX
				Matrix3D B_rot({ {  cos_b,    sin_b,    0   },
									{ -sin_b,    cos_b,    0   },
									{    0,        0,      1   } });

				float cos_tetha = cos(tetha * PI / 180.0f);
				float sin_tetha = sin(tetha * PI / 180.0f);

				Vector3 pos(R2 + R1 * cos_tetha, R1 * sin_tetha, 0);

				Matrix3D all_rot = torus_rot * A_rot * B_rot;
				pos = pos * all_rot;

				//x', y' <--- projection
				//echo(pos.x, pos.y, pos.z);
				int xp = K1 * 2.1f * (pos.x / (pos.z + K2));
				int yp = K1 * (pos.y / (pos.z + K2));
				//echo(xp, yp);

				if (xp < -W / 2 || W / 2 < xp
					|| yp < -H / 2 || H / 2 < yp) {
					cerr << "xp yp POZA EKRANEM!\n";
					debug system("pause");
					cerr << "-----------------------------\n";
				}

				if (pos.z < zbuff[xp + W / 2][yp + H / 2]) {
					zbuff[xp + W / 2][yp + H / 2] = pos.z;
					//obliczamy (Nx, Ny, Nz)

					Vector3 N(cos_tetha, sin_tetha, 0);
					N = N * all_rot;
					//echo(N.x, N.y, N.z);
					N = N.normalize();

					if (float d = dot(N, Light); d <= 0) {
						//echo("Lamar: Nigger!");

						//to znaczy ze swiecimy na ten piksel
						//PRAWDOPODOBNIE -d <0, 1>, bo d = cos*|N|*|Light|
						int color_type = -d * 12;//0...11(12 moze?)
						assert(0 <= color_type && color_type <= 12);
						color[xp + W / 2][yp + H / 2] = ".,-~:;=!*#$@8"[color_type];//fajny trik
					}
				}
			}
		}

		system("cls");//system("clear") for linux
		DFOR(y, H - 1, 0) {
			FOR(x, 0, W - 1)
				putchar(color[x][y]);
			putchar('\n');//cout << "\n";
		}
		//cout << flush;
		//debug system("pause");
	}
	return 0;
}
