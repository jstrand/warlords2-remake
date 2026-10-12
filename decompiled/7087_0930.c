
int __cdecl16far city_compute_defence(int city)

{
  int iVar1;
  
  iVar1 = city_production_count(city);
  if (iVar1 < 3) {
    iVar1 = 1;
  }
  else {
    iVar1 = 2;
  }
  return iVar1;
}


