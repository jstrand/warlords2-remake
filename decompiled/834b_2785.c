
void __cdecl16far FUN_834b_2785(int param_1,int param_2,int param_3)

{
  char cVar1;
  undefined2 uVar2;
  int iVar3;
  undefined2 in_DX;
  undefined2 uVar4;
  bool bVar5;
  int local_8;
  uint local_4;
  
  if (param_1 != 0) {
    auto_file_mapcolor_dat();
  }
  DAT_451b_1f06 = DAT_451b_1f12;
  uVar4 = 0x1997;
  FUN_1997_034a();
  if ((DAT_451b_18c7 != 0) && ((DAT_451b_18f2 & 10) == 0)) {
    for (local_8 = 0; local_8 < (int)(uint)DAT_451b_18f1; local_8 = local_8 + 1) {
      uVar4 = 0x1d28;
      uVar2 = mem_lock(((undefined4 *)&DAT_451b_18c7)[local_8]);
      ((undefined2 *)&DAT_451b_18d9)[local_8 * 2] = in_DX;
      ((undefined2 *)&DAT_451b_18d7)[local_8 * 2] = uVar2;
    }
  }
  local_4 = (uint)*(byte *)(param_3 * 0xe0 + param_2 * 2);
  bVar5 = (*(byte *)(param_3 * 0x70 + param_2) & 0x1f) != 0;
  if (bVar5) {
    local_4 = (*(byte *)(param_3 * 0x70 + param_2) & 0x1f) - 1;
  }
  if (((int)local_4 < 0x50) || (0x5f < (int)local_4)) {
    if (bVar5) {
      cVar1 = *(char *)((int)DAT_451b_1f22 + local_4);
    }
    else {
      cVar1 = *(char *)((int)DAT_451b_1f32 + local_4);
    }
    (*DAT_451b_2a70)(uVar4,0x581d,0x4125,param_2 << 1,param_3 << 1,
                     *(undefined2 *)((int)DAT_451b_1f06 + cVar1 * 2));
    if (bVar5) {
      cVar1 = *(char *)((int)DAT_451b_1f1e + local_4);
    }
    else {
      cVar1 = *(char *)((int)DAT_451b_1f2e + local_4);
    }
    (*DAT_451b_2a70)(uVar4,0x581d,0x4125,param_2 * 2 + 1,param_3 << 1,
                     *(undefined2 *)((int)DAT_451b_1f06 + cVar1 * 2));
    if (bVar5) {
      cVar1 = *(char *)((int)DAT_451b_1f1a + local_4);
    }
    else {
      cVar1 = *(char *)((int)DAT_451b_1f2a + local_4);
    }
    (*DAT_451b_2a70)(uVar4,0x581d,0x4125,param_2 << 1,param_3 * 2 + 1,
                     *(undefined2 *)((int)DAT_451b_1f06 + cVar1 * 2));
    if (bVar5) {
      cVar1 = *(char *)((int)DAT_451b_1f16 + local_4);
    }
    else {
      cVar1 = *(char *)((int)DAT_451b_1f26 + local_4);
    }
    (*DAT_451b_2a70)(uVar4,0x581d,0x4125,param_2 * 2 + 1,param_3 * 2 + 1,
                     *(undefined2 *)((int)DAT_451b_1f06 + cVar1 * 2));
    if (param_1 != 0) {
      FUN_834b_2bb6();
    }
  }
  else {
    iVar3 = dice(1,3,1);
    (*DAT_451b_2a70)(0x7ecb,0x581d,0x4125,param_2 << 1,param_3 << 1,iVar3);
    iVar3 = dice(1,3,1);
    (*DAT_451b_2a70)(0x7ecb,0x581d,0x4125,param_2 * 2 + 1,param_3 << 1,iVar3);
    iVar3 = dice(1,3,1);
    (*DAT_451b_2a70)(0x7ecb,0x581d,0x4125,param_2 << 1,param_3 * 2 + 1,iVar3);
    iVar3 = dice(1,3,1);
    (*DAT_451b_2a70)(0x7ecb,0x581d,0x4125,param_2 * 2 + 1,param_3 * 2 + 1,iVar3);
  }
  return;
}


