
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_8611_0d39(void)

{
  byte *pbVar1;
  byte bVar2;
  byte bVar3;
  byte *pbVar4;
  int iVar5;
  undefined2 in_DX;
  int iVar6;
  byte *local_8;
  
  pbVar4 = (byte *)mem_lock(_LAB_4125_126f_1);
  _LAB_4125_126b_1 = (byte *)CONCAT22(in_DX,pbVar4);
  local_8 = _LAB_4125_126b_1;
  bVar3 = (byte)current_player & 0x1f;
  for (iVar6 = 0; iVar6 < 0x9c; iVar6 = iVar6 + 1) {
    for (iVar5 = 0; pbVar1 = local_8, iVar5 < 0x70; iVar5 = iVar5 + 1) {
      local_8 = (byte *)CONCAT22(local_8._2_2_,(byte *)local_8 + 1);
      bVar2 = *pbVar1;
      pbVar4 = (byte *)(iVar6 * 0x70 + iVar5);
      pbVar1 = pbVar4;
      *pbVar1 = *pbVar1 & 0xdf;
      pbVar1 = pbVar4;
      *pbVar1 = *pbVar1 | ((1 << bVar3 & (uint)bVar2) != 0) << 5;
    }
  }
  mem_unlock(_LAB_4125_126f_1);
  return;
}


