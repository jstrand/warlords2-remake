
void __cdecl16far FUN_5c52_0144(int param_1,int param_2,int param_3,undefined1 param_4,int param_5)

{
  byte *pbVar1;
  int iVar2;
  undefined1 *puVar3;
  uint uVar4;
  undefined4 local_e;
  undefined4 local_a;
  undefined4 local_6;
  
  if (((*DAT_4125_34e7 == 1) || (*DAT_4125_34e7 == 2)) || (*DAT_4125_34e7 == 4)) {
    iVar2 = 0x50;
  }
  else {
    iVar2 = 0x28;
  }
  out(0x3ce,5);
  out(0x3cf,2);
  out(0x3c4,2);
  out(0x3c5,0xf);
  out(0x3ce,8);
  puVar3 = (undefined1 *)(param_2 * 8 * iVar2 + param_1);
  local_6 = (undefined1 *)CONCAT22(0xa000,puVar3);
  local_a = (byte *)CONCAT22(0xf000,(byte *)(param_3 * 8 + -0x592));
  out(0x3cf,0xff);
  local_e = (undefined1 *)CONCAT22(0xa000,puVar3 + iVar2 * 7);
  *local_e = 0;
  if (param_5 == 0) {
    for (uVar4 = 0; pbVar1 = local_a, uVar4 < 8; uVar4 = uVar4 + 1) {
      local_a = (byte *)CONCAT22(local_a._2_2_,(byte *)local_a + 1);
      out(0x3cf,*pbVar1);
      *local_6 = param_4;
      local_6 = (undefined1 *)CONCAT22(local_6._2_2_,(undefined1 *)local_6 + iVar2);
    }
  }
  else {
    for (uVar4 = 0; pbVar1 = local_a, uVar4 < 8; uVar4 = uVar4 + 1) {
      local_a = (byte *)CONCAT22(local_a._2_2_,(byte *)local_a + 1);
      out(0x3cf,*pbVar1 ^ 0xff);
      *local_6 = param_4;
      local_6 = (undefined1 *)CONCAT22(local_6._2_2_,(undefined1 *)local_6 + iVar2);
    }
  }
  FUN_5c52_0247();
  return;
}


