
void __cdecl16far FUN_5311_0000(void)

{
  byte *pbVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 in_DX;
  int iVar4;
  undefined2 local_6;
  undefined2 local_4;
  
  uVar2 = mem_alloc(700,0,0,0);
  DAT_451b_002e = CONCAT22(in_DX,uVar2);
  uVar2 = mem_lock(uVar2,in_DX);
  DAT_451b_0032 = CONCAT22(in_DX,uVar2);
  random_map_progress();
  FUN_4fef_0000();
  for (iVar4 = 0; iVar4 < 0x70; iVar4 = iVar4 + 1) {
    for (iVar3 = 0; iVar3 < 0x9c; iVar3 = iVar3 + 1) {
      pbVar1 = (byte *)(iVar3 * 0xe0 + iVar4 * 2 + 1);
      *pbVar1 = *pbVar1 & 0x7f;
      pbVar1 = (byte *)(iVar3 * 0x70 + iVar4);
      *pbVar1 = *pbVar1 & 0xe0;
    }
  }
  random_map_progress();
  FUN_5311_0165(DAT_451b_0032);
  iVar4 = 100;
  while (iVar4 != 0) {
    *(undefined1 *)((int)DAT_451b_0032 + iVar4 + -1 + 600) = 0;
    iVar4 = iVar4 + -1;
  }
  random_map_progress();
  iVar4 = 4;
  while( true ) {
    iVar3 = FUN_5311_0e22((int)DAT_451b_0032,(int)((ulong)DAT_451b_0032 >> 0x10),&local_4);
    if (iVar3 == 0) break;
    if (iVar4 < 10) {
      iVar3 = dice(1,3,-1);
      if (iVar3 == 0) {
        iVar4 = iVar4 + 1;
        random_map_progress();
      }
    }
    path_build_cost_grid(0xe,1);
    FUN_5311_0b86((int)DAT_451b_0032,(int)((ulong)DAT_451b_0032 >> 0x10),local_4,local_6);
  }
  FUN_1d28_048e(DAT_451b_002e);
  for (iVar4 = 0; iVar4 < 0x9c; iVar4 = iVar4 + 1) {
    for (iVar3 = 0; iVar3 < 0x70; iVar3 = iVar3 + 1) {
      if (*(char *)((int)DAT_451b_002a + iVar4 * 0x70 + iVar3 + 0x6120) == '\v') {
        pbVar1 = (byte *)(iVar4 * 0x70 + iVar3);
        *pbVar1 = *pbVar1 & 0xe0;
      }
    }
  }
  return;
}


