
void __cdecl16far random_map_marshes(void)

{
  int iVar1;
  int iVar2;
  
  iVar1 = dice(1,3,0);
  for (iVar2 = 0; iVar2 < iVar1; iVar2 = iVar2 + 1) {
    random_map_one_marsh();
  }
  return;
}


