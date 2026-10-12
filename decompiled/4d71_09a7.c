
void __cdecl16far
FUN_4d71_09a7(undefined4 param_1,undefined4 param_2,int param_3,undefined1 param_4)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  int local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  undefined4 local_8;
  int local_4;
  
  local_4 = 0;
  local_8 = param_1;
  FUN_4c49_0dd0(param_1,param_2,&local_c);
  while (local_4 == 0) {
    FUN_4c49_0dd0(local_8,param_2,&local_10);
    iVar1 = FUN_4c49_0fed(local_8,param_2);
    if ((iVar1 < 3) || (iVar1 = dice(1,3,0), iVar1 == 1)) {
      local_8 = CONCAT22(local_8._2_2_ + local_10._2_2_,(int)local_8 + (int)local_10);
      local_14 = local_10;
    }
    else if (iVar1 == 2) {
      local_8 = CONCAT22(local_8._2_2_ + local_c._2_2_,(int)local_8 + (int)local_c);
      local_14 = local_c;
    }
    else if (iVar1 == 3) {
      FUN_4c49_104a(&local_10);
      local_8 = CONCAT22(local_8._2_2_ + local_10._2_2_,(int)local_8 + (int)local_10);
      local_c = local_10;
      local_14 = local_10;
    }
    iVar1 = 0;
    while( true ) {
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      if (7 < iVar1) break;
      iVar2 = (int)DAT_451b_002a + iVar1 * 4;
      if ((*(int *)(iVar2 + 0xbc) == (int)local_14) && (*(int *)(iVar2 + 0xbe) == local_14._2_2_)) {
        local_28 = iVar1;
      }
      iVar1 = iVar1 + 1;
    }
    iVar1 = (local_28 + 2) % 8;
    iVar2 = (local_28 + 6) % 8;
    iVar3 = (int)DAT_451b_002a + iVar1 * 4;
    local_18 = CONCAT22(local_8._2_2_ + *(int *)(iVar3 + 0xbe),(int)local_8 + *(int *)(iVar3 + 0xbc)
                       );
    iVar3 = (int)DAT_451b_002a + iVar2 * 4;
    local_1c = CONCAT22(local_8._2_2_ + *(int *)(iVar3 + 0xbe),(int)local_8 + *(int *)(iVar3 + 0xbc)
                       );
    iVar1 = (int)DAT_451b_002a + iVar1 * 4;
    local_20 = CONCAT22(local_8._2_2_ + *(int *)(iVar1 + 0xbe) * 2,
                        (int)local_8 + *(int *)(iVar1 + 0xbc) * 2);
    iVar2 = (int)DAT_451b_002a + iVar2 * 4;
    iVar1 = (int)local_8 + *(int *)(iVar2 + 0xbc) * 2;
    iVar2 = local_8._2_2_ + *(int *)(iVar2 + 0xbe) * 2;
    local_24 = CONCAT22(iVar2,iVar1);
    FUN_4bed_0470(&local_8);
    uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar3 = (int)DAT_451b_002a;
    if ((*(char *)(iVar3 + local_8._2_2_ * 0x70 + (int)local_8 + 0x6120) == '\a') ||
       (*(char *)(iVar3 + local_8._2_2_ * 0x70 + (int)local_8 + 0x6120) == '\x05')) {
      *(undefined1 *)(iVar3 + local_8._2_2_ * 0x70 + (int)local_8 + 0x6120) = param_4;
      FUN_4bed_03aa(local_8);
    }
    if ((int)local_8 == (int)param_2) {
      if (local_8._2_2_ == param_2._2_2_) {
        local_4 = 1;
      }
    }
    if (param_3 == 2) {
      FUN_4bed_0470(&local_18);
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = (int)DAT_451b_002a;
      if ((*(char *)(iVar3 + local_18._2_2_ * 0x70 + (int)local_18 + 0x6120) == '\a') ||
         (*(char *)(iVar3 + local_18._2_2_ * 0x70 + (int)local_18 + 0x6120) == '\x05')) {
        *(undefined1 *)(iVar3 + local_18._2_2_ * 0x70 + (int)local_18 + 0x6120) = param_4;
        FUN_4bed_03aa(local_18);
      }
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = (int)DAT_451b_002a;
      if ((*(char *)(iVar3 + local_1c._2_2_ * 0x70 + (int)local_1c + 0x6120) == '\a') ||
         (*(char *)(iVar3 + local_1c._2_2_ * 0x70 + (int)local_1c + 0x6120) == '\x05')) {
        *(undefined1 *)(iVar3 + local_1c._2_2_ * 0x70 + (int)local_1c + 0x6120) = param_4;
        FUN_4bed_03aa(local_1c);
      }
    }
    if (param_3 == 3) {
      FUN_4bed_0470(&local_20);
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = (int)DAT_451b_002a;
      if ((*(char *)(iVar3 + local_20._2_2_ * 0x70 + (int)local_20 + 0x6120) == '\a') ||
         (*(char *)(iVar3 + local_20._2_2_ * 0x70 + (int)local_20 + 0x6120) == '\x05')) {
        *(undefined1 *)(iVar3 + local_20._2_2_ * 0x70 + (int)local_20 + 0x6120) = param_4;
        FUN_4bed_03aa(local_20);
      }
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = (int)DAT_451b_002a;
      if ((*(char *)(iVar3 + iVar2 * 0x70 + iVar1 + 0x6120) == '\a') ||
         (*(char *)(iVar3 + iVar2 * 0x70 + iVar1 + 0x6120) == '\x05')) {
        *(undefined1 *)(iVar3 + iVar2 * 0x70 + iVar1 + 0x6120) = param_4;
        FUN_4bed_03aa(local_24);
      }
    }
  }
  return;
}


