
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_8611_0dc7(byte param_1)

{
  byte *pbVar1;
  byte bVar2;
  byte *pbVar3;
  int iVar4;
  undefined2 in_DX;
  int iVar5;
  byte *local_8;
  
  FUN_8611_0e68();
  pbVar3 = (byte *)mem_lock(_LAB_4125_126f_1);
  _LAB_4125_126b_1 = (byte *)CONCAT22(in_DX,pbVar3);
  local_8 = _LAB_4125_126b_1;
  for (iVar5 = 0; iVar5 < 0x9c; iVar5 = iVar5 + 1) {
    for (iVar4 = 0; pbVar1 = local_8, iVar4 < 0x70; iVar4 = iVar4 + 1) {
      local_8 = (byte *)CONCAT22(local_8._2_2_,(byte *)local_8 + 1);
      bVar2 = *pbVar1;
      pbVar3 = (byte *)(iVar5 * 0x70 + iVar4);
      pbVar1 = pbVar3;
      *pbVar1 = *pbVar1 & 0xdf;
      pbVar1 = pbVar3;
      *pbVar1 = *pbVar1 | ((1 << (param_1 & 0x1f) & (uint)bVar2) != 0) << 5;
    }
  }
  mem_unlock(_LAB_4125_126f_1);
  DAT_4125_2d28 = 1;
  return;
}


