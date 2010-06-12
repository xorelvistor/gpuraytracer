class Vector3
{
	public:
		Vector3() : x( 0.0f ), y( 0.0f ), z( 0.0f ) {};
		Vector3( float a_X, float a_Y, float a_Z ) : x( a_X ), y( a_Y ), z( a_Z ) {};
		void Set( float a_X, float a_Y, float a_Z ) { x = a_X; y = a_Y; z = a_Z; }
		void Normalize() { float l = 1.0f / Length(); x *= l; y *= l; z *= l; }
		float Length() { return (float)sqrt( x * x + y * y + z * z ); }
		float SqrLength() { return x * x + y * y + z * z; }
		float Dot( Vector3 a_V ) { return x * a_V.x + y * a_V.y + z * a_V.z; }
		Vector3 Cross( Vector3 b ) { return Vector3( y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x ); }
		void operator += ( Vector3& a_V ) { x += a_V.x; y += a_V.y; z += a_V.z; }
		void operator += ( Vector3* a_V ) { x += a_V->x; y += a_V->y; z += a_V->z; }
		void operator -= ( Vector3& a_V ) { x -= a_V.x; y -= a_V.y; z -= a_V.z; }
		void operator -= ( Vector3* a_V ) { x -= a_V->x; y -= a_V->y; z -= a_V->z; }
		void operator *= ( float f ) { x *= f; y *= f; z *= f; }
		void operator *= ( Vector3& a_V ) { x *= a_V.x; y *= a_V.y; z *= a_V.z; }
		void operator *= ( Vector3* a_V ) { x *= a_V->x; y *= a_V->y; z *= a_V->z; }
		Vector3 operator- () const { return Vector3( -x, -y, -z ); }
		friend Vector3 operator + ( const Vector3& v1, const Vector3& v2 ) { return Vector3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z ); }
		friend Vector3 operator - ( const Vector3& v1, const Vector3& v2 ) { return Vector3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z ); }
		friend Vector3 operator + ( const Vector3& v1, Vector3* v2 ) { return Vector3( v1.x + v2->x, v1.y + v2->y, v1.z + v2->z ); }
		friend Vector3 operator - ( const Vector3& v1, Vector3* v2 ) { return Vector3( v1.x - v2->x, v1.y - v2->y, v1.z - v2->z ); }
		friend Vector3 operator * ( const Vector3& v, float f ) { return Vector3( v.x * f, v.y * f, v.z * f ); }
		friend Vector3 operator * ( const Vector3& v1, Vector3& v2 ) { return Vector3( v1.x * v2.x, v1.y * v2.y, v1.z * v2.z ); }
		friend Vector3 operator * ( float f, const Vector3& v ) { return Vector3( v.x * f, v.y * f, v.z * f ); }
		union
		{
			struct { float x, y, z; };
			struct { float red, green, blue; };
			struct { float cell[3]; };
		};
};