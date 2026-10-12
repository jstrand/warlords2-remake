
void __cdecl16far
FUN_5c52_0099(int param_1,undefined2 param_2,uint param_3,int param_4,undefined2 param_5,
             undefined2 param_6)

{
  bool bVar1;
  char *pcVar2;
  uint uVar3;
  undefined2 unaff_SS;
  int local_a;
  char local_8 [6];
  
  pcVar2 = (char *)s__x__Use_extended_memory_4125_2703 + 0xd;
  for (uVar3 = 0; uVar3 < 5; uVar3 = uVar3 + 1) {
    local_a._0_1_ = (char)(param_3 / (uint)pcVar2);
    local_8[uVar3] = (char)local_a + '0';
    param_3 = param_3 - (param_3 / (uint)pcVar2) * (int)pcVar2;
    pcVar2 = (char *)((uint)pcVar2 / 10);
  }
  local_a = 0;
  bVar1 = false;
  for (uVar3 = 5 - param_4; uVar3 < 5; uVar3 = uVar3 + 1) {
    if (((uVar3 == 4) || (local_8[uVar3] != '0')) || (bVar1)) {
      FUN_5c52_0144(param_1 + local_a,param_2,local_8[uVar3],param_5,param_6);
      bVar1 = true;
    }
    else {
      FUN_5c52_0144(param_1 + local_a,param_2,0x20,param_5,param_6);
    }
    local_a = local_a + 1;
  }
  return;
}


