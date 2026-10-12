
void __cdecl16far random_map_one_marsh(void)

{
  bool bVar1;
  int iVar2;
  int unaff_SI;
  int unaff_DI;
  undefined2 local_4;
  
  local_4 = 0;
  bVar1 = false;
  while (!bVar1) {
    unaff_SI = dice(1,0x66,5);
    unaff_DI = dice(1,0x92,5);
    if (*(char *)((int)DAT_451b_002a + unaff_DI * 0x70 + unaff_SI + 0x6120) == '\a') {
      if (local_4 < 4) {
        iVar2 = terrain_near(unaff_SI,unaff_DI,3);
        if (iVar2 != 0) {
          bVar1 = true;
        }
        iVar2 = terrain_near(unaff_SI + 1,unaff_DI,3);
        if (iVar2 != 0) {
          bVar1 = true;
        }
        iVar2 = terrain_near(unaff_SI,unaff_DI + 1,3);
        if (iVar2 != 0) {
          bVar1 = true;
        }
        iVar2 = terrain_near(unaff_SI + 1,unaff_DI + 1,3);
        if (iVar2 != 0) {
          bVar1 = true;
        }
        local_4 = local_4 + 1;
      }
      else {
        bVar1 = true;
      }
    }
  }
  *(undefined1 *)((int)DAT_451b_002a + unaff_DI * 0x70 + unaff_SI + 0x6120) = 8;
  FUN_4bed_03aa(unaff_SI,unaff_DI);
  FUN_4fc9_0029(unaff_SI,unaff_DI);
  FUN_4fc9_0029(unaff_SI + 1,unaff_DI + 1);
  FUN_4fc9_0029(unaff_SI + -1,unaff_DI + -1);
  FUN_4fc9_0029(unaff_SI + -1,unaff_DI + 1);
  FUN_4fc9_0029(unaff_SI + 1,unaff_DI + -1);
  return;
}


