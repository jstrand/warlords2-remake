
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far hero_brings_allies(void)

{
  int *piVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 unaff_SS;
  char *pcVar5;
  char local_3c [50];
  undefined2 local_a;
  int local_8;
  undefined2 local_6;
  int local_4;
  
  FUN_834b_0000(1,0,0,0);
  piVar1 = (int *)(current_player * 0x14 + 0x185);
  *piVar1 = *piVar1 - _LAB_4125_110f_1;
  _LAB_4125_110f_1 = 0;
  iVar2 = game_turn();
  if (iVar2 != 1) {
    local_4 = 1;
    local_6 = random_magical_type();
    iVar2 = dice(1,100,0);
    if (iVar2 < 0x46) {
      iVar4 = 1;
    }
    else if (iVar2 < 0x5f) {
      iVar4 = 2;
    }
    else {
      iVar4 = 3;
    }
    if (local_4 != 0) {
      iVar3 = is_computer_turn();
      if (iVar3 == 0) {
        if (iVar4 == 1) {
          pcVar5 = get_string(0x60,0);
          iVar2 = (int)((ulong)pcVar5 >> 0x10);
          sprintf((char *)CONCAT22(unaff_SS,local_3c),pcVar5);
        }
        else {
          iVar3 = iVar4;
          pcVar5 = get_string(0x60,1);
          iVar2 = (int)((ulong)pcVar5 >> 0x10);
          sprintf((char *)CONCAT22(unaff_SS,local_3c),pcVar5,iVar3);
        }
        FUN_8065_10fb(local_3c);
      }
      for (iVar3 = 0; iVar3 < iVar4; iVar3 = iVar3 + 1) {
        local_a = create_ally_army(local_6,DAT_451b_058a,DAT_451b_0588);
        local_8 = iVar2;
        FUN_1b62_02e2(local_a,iVar2);
      }
    }
  }
  return;
}


