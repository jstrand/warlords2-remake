
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_8611_113b(void)

{
  byte *pbVar1;
  undefined1 *puVar2;
  undefined1 uVar3;
  undefined2 in_DX;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined1 *local_e;
  int local_a;
  int local_4;
  
  puVar2 = (undefined1 *)mem_lock(_LAB_4125_126f_1);
  _LAB_4125_126b_1 = (undefined1 *)CONCAT22(in_DX,puVar2);
  local_e = _LAB_4125_126b_1;
  if (opt_hidden_map == 0) {
    uVar3 = 0xff;
  }
  else {
    uVar3 = 0;
  }
  for (iVar5 = 0; iVar5 < 0x9c; iVar5 = iVar5 + 1) {
    for (iVar6 = 0; iVar6 < 0x70; iVar6 = iVar6 + 1) {
      *local_e = uVar3;
      local_e = (undefined1 *)CONCAT22(local_e._2_2_,(undefined1 *)local_e + 1);
      pbVar1 = (byte *)(iVar5 * 0x70 + iVar6);
      *pbVar1 = *pbVar1 | 0x20;
      pbVar1 = (byte *)(iVar5 * 0x70 + iVar6);
      *pbVar1 = *pbVar1 & 0x7f;
    }
  }
  for (iVar5 = 0; iVar5 < 9; iVar5 = iVar5 + 1) {
    for (iVar6 = 0; iVar6 < 9; iVar6 = iVar6 + 1) {
      ((undefined1 *)&DAT_451b_1f74)[iVar5 * 9 + iVar6] = 0xff;
    }
  }
  for (local_4 = 0; local_4 < 8; local_4 = local_4 + 1) {
    for (local_a = 0; local_a < city_count; local_a = local_a + 1) {
      if (*(char *)(local_a * 0x41 + 0x1592) == local_4) {
        iVar5 = *(int *)(local_a * 0x41 + 0x157d);
        iVar6 = *(int *)(local_a * 0x41 + 0x157f);
        for (iVar4 = iVar5 + -2; iVar4 <= iVar5 + 3; iVar4 = iVar4 + 1) {
          for (iVar7 = iVar6 + -2; iVar7 <= iVar6 + 3; iVar7 = iVar7 + 1) {
            _LAB_4125_126b_1[iVar4 + iVar7 * 0x70] =
                 _LAB_4125_126b_1[iVar4 + iVar7 * 0x70] | '\x01' << ((byte)local_4 & 0x1f);
          }
        }
      }
    }
  }
  mem_unlock(_LAB_4125_126f_1);
  return;
}


