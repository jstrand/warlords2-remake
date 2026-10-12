
void __cdecl16far FUN_4d71_03af(void)

{
  bool bVar1;
  int unaff_SI;
  int unaff_DI;
  undefined2 uVar2;
  int local_4;
  
  for (local_4 = 0; local_4 < *(int *)((int)DAT_451b_002a + 0x36); local_4 = local_4 + 1) {
    bVar1 = false;
    while (!bVar1) {
      unaff_SI = dice(1,0x70,-1);
      unaff_DI = dice(1,0x9c,-1);
      if (*(char *)((int)DAT_451b_002a + unaff_DI * 0x70 + unaff_SI + 0x6120) == '\a') {
        bVar1 = true;
      }
    }
    uVar2 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    *(undefined1 *)((int)DAT_451b_002a + unaff_DI * 0x70 + unaff_SI + 0x6120) = 5;
    *(int *)((int)DAT_451b_002a + *(int *)((int)DAT_451b_002a + 0x611e) * 0xe + 0x5dd6) = unaff_SI;
    *(int *)((int)DAT_451b_002a + *(int *)((int)DAT_451b_002a + 0x611e) * 0xe + 0x5dd8) = unaff_DI;
    *(undefined2 *)
     ((int)(undefined2 *)&side_upkeep +
     (int)DAT_451b_002a + *(int *)((int)DAT_451b_002a + 0x611e) * 0xe) = 1;
    *(undefined2 *)((int)DAT_451b_002a + *(int *)((int)DAT_451b_002a + 0x611e) * 0xe + 0x5ddc) = 0;
    *(int *)((int)DAT_451b_002a + 0x611e) = *(int *)((int)DAT_451b_002a + 0x611e) + 1;
    FUN_4bed_03aa(unaff_SI,unaff_DI);
  }
  return;
}


