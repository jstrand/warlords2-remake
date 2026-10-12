
void __cdecl16far FUN_563e_16fd(int param_1)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 uVar6;
  undefined2 unaff_SS;
  int local_26 [7];
  byte abStack_17 [6];
  byte abStack_11 [7];
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  uVar6 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  local_8 = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + 0x250);
  local_a = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + 0x2a4);
  for (local_4 = 0; local_4 < 6; local_4 = local_4 + 1) {
    iVar1 = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + local_4 * 2 + 0x292);
    if (((iVar1 != -1) && (iVar1 != local_8)) &&
       (*(char *)(iVar1 * 0x41 + 0x1592) == current_player)) {
      FUN_5db9_0656(iVar1,0);
      iVar3 = 0;
      iVar5 = 6;
      while (iVar4 = iVar5, iVar5 = iVar4 + -1, iVar4 != 0) {
        uVar2 = (uint)abStack_11[iVar4];
        if (((uVar2 != 0xff) && (*(char *)(uVar2 * 0x41 + 0x1592) != '\x0f')) &&
           ((*(char *)(uVar2 * 0x41 + 0x1592) != current_player && (abStack_17[iVar4] < 0x19)))) {
          iVar3 = iVar3 + 1;
        }
      }
      if (iVar3 == 0) {
        ai_city_garrison_check();
        local_6 = ai_collect_stack(*(int *)(iVar1 * 0x41 + 0x157d) + 1,
                                   *(int *)(iVar1 * 0x41 + 0x157f),
                                   (int *)CONCAT22(unaff_SS,local_26),0,0,local_a);
        if (3 < local_6) {
          FUN_623c_0ae7(local_26);
          move_stack_to();
        }
      }
    }
  }
  return;
}


