
void __cdecl16far FUN_4eb7_0000(void)

{
  int iVar1;
  
  for (iVar1 = 0; iVar1 < *(int *)((int)DAT_451b_002a + 0x3a); iVar1 = iVar1 + 1) {
    FUN_4eb7_005d(0,1);
  }
  for (iVar1 = 0; iVar1 < *(int *)((int)DAT_451b_002a + 0x38); iVar1 = iVar1 + 1) {
    FUN_4eb7_005d(1,1);
  }
  FUN_4eb7_060c();
  FUN_4c49_0ce7();
  random_map_smooth_highlands();
  FUN_4d71_033a();
  FUN_4eb7_060c();
  FUN_4c49_0ce7();
  return;
}


