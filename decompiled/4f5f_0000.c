
void __cdecl16far FUN_4f5f_0000(void)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < *(int *)((int)DAT_451b_002a + 0x40); iVar1 = iVar1 + 1) {
    random_map_walk_line(0);
  }
  for (iVar1 = 0; iVar1 < *(int *)((int)DAT_451b_002a + 0x3e); iVar1 = iVar1 + 1) {
    random_map_walk_line(1);
  }
  FUN_4f5f_04d6();
  FUN_4f5f_05ae();
  FUN_4d71_033a();
  return;
}


