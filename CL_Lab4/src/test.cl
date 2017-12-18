//скалярное произведение векторов
float mscalar (float3 a, float3 b)
{

return ((a.x*b.x) + (a.y*b.y) + (a.z*b.z));
}

//векторное произведение векторов
float3 mvector(float3 a, float3 b)
{
float i, j, k;
i = a.y*b.z - a.z*b.y;
j = a.z*b.x - a.x*b.z;
k = a.x*b.y - a.y*b.x;
float3 v;
v.x = i;
v.y = j;
v.z = k;
return v;
}

//Модуль вектора
float modvector(float3 a)
{
return sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
}

//Проекция вектора a на направление вектора b
float prvector(float3 a, float3 b)
{
float retval;
float modb = sqrt(b.x*b.x+b.y*b.y+b.z*b.z);
retval = mscalar(a,b)/modb;
return retval;
}

float3 normvector(float3 a)
{
float3 ret;
float len = modvector(a);
ret.x = a.x/len;
ret.y = a.y/len;
ret.z = a.z/len;
return ret;
}



__kernel void test(__global float3* u, __global float3* p, __global float3* a, __global float3* b, __global float3* c)
{
// получаем текущий id.
int id = get_global_id(0);
float3 uv = u[id];
float3 pv = p[id];

float3 av = a[id];
float3 bv = b[id];
float3 cv = c[id];

a[id] = u[id];
uv = normvector(uv);

//Вектор ab
float3 abv = bv-av;
//Вектор ac
float3 acv = cv - av;
//Нормаль к плоскости
float3 nv = normvector(mvector(abv, acv));
//Скалярное произведение нормали и вектора луча
float un_s = mscalar(nv, uv);
//Случай, когда луч параллелен плоскости
if(un_s==0)
{

uv.x = 0;
uv.y = 1;
uv.z = 0;
u[id] = uv;

p[id].x = un_s;

return;
}

//Число d(расстояние от плоскости до начала координат)
//Определяется проекцией любой точки, принадлежащей плоскости, на вектор нормали.
float d_s = prvector(av, -nv);
//Число pdd - расстояние от точки до плоскости вдоль обратного вектора нормали
float pdz_s = prvector(pv, -nv);
float pdd_s = pdz_s-d_s;
float pv_s = mscalar(pv,nv);
a[id].x = pv_s;
float alpha_s = (-d_s - pv_s)/un_s;
//Случай, когда луч направлен от плоскости
if(alpha_s<=0)
{
uv.x = 0;
uv.y = 0;
uv.z = 1;
u[id] = uv;
return;
}
a[id].y = alpha_s;
//Точка пересечения луча и плоскости
float3 auv;
auv.x = alpha_s*uv.x;
auv.y = alpha_s*uv.y;
auv.z = alpha_s*uv.z;
float3 i_v;// = pv + auv;
i_v.x = pv.x+auv.x;
i_v.y = pv.y+auv.y;
i_v.z = pv.z+auv.z;
float3 U,V,F, T;
float Um, Vm, Fm, Tm;
T = mvector(bv-av, cv-av);
U = mvector(i_v-av, i_v-bv);
V = mvector(i_v-bv, i_v-cv);
F = mvector(i_v-av, i_v-cv);

//Модули площадей параллелограммов
Tm = modvector(T);
Um = modvector(U);
Vm = modvector(V);
Fm = modvector(F);

//Точка пересечения с плоскостью за пределами треугольника
if(((Um+Vm)>=Tm)||((Fm+Vm)>=Tm)||((Um+Fm)>=Tm))
{
uv.x = 0;
uv.y = 1;
uv.z = 1;
u[id] = uv;
//i_v.x = Um;
//i_v.y = Vm;
//i_v.z = Tm;
p[id].x = d_s;
p[id].y = alpha_s;
return;
}
//Точка пересечения внутри треугольника;
uv.x = uv.y = uv.z = 1;
u[id] = uv;
p[id] = i_v;
//a[id] = p[id];


}
