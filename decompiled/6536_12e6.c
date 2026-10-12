
int * __cdecl16far create_ally_army(undefined2 param_1,int param_2,int param_3)

{
  byte *pbVar1;
  byte bVar2;
  int iVar3;
  int *piVar4;
  int in_DX;
  int iVar5;
  int y;
  undefined2 unaff_SS;
  undefined1 local_4a [22];
  undefined1 local_34;
  undefined1 local_2e;
  int local_c;
  int local_a;
  int local_8;
  undefined4 local_6;
  
  local_c = 10;
  armytype_lookup(param_1,local_4a);
  local_8 = 0;
  iVar5 = param_2;
  y = param_3;
  do {
    while( true ) {
      if (local_8 != 0) {
        piVar4 = (int *)FUN_1b62_0309();
        local_6 = (int *)CONCAT22(in_DX,piVar4);
        *local_6 = param_2;
        piVar4[1] = param_3;
        *(undefined1 *)(piVar4 + 2) = (undefined1)param_1;
        *(undefined1 *)((int)piVar4 + 5) = (undefined1)current_player;
        *(undefined1 *)(piVar4 + 3) = local_2e;
        *(undefined1 *)((int)piVar4 + 7) = local_2e;
        *(undefined1 *)(piVar4 + 4) = local_34;
        *(undefined1 *)((int)piVar4 + 9) = 0;
        *(undefined1 *)(piVar4 + 5) = 0;
        *(undefined1 *)((int)piVar4 + 0xb) = 0;
        FUN_1b62_02e2(piVar4,in_DX);
        bVar2 = (byte)current_player;
        iVar5 = param_3 * 0xe0 + param_2 * 2;
        pbVar1 = (byte *)(iVar5 + 1);
        *pbVar1 = *pbVar1 & 0xf0;
        pbVar1 = (byte *)(iVar5 + 1);
        *pbVar1 = *pbVar1 | bVar2 & 0xf;
        return (int *)local_6;
      }
      armies_at_tile(iVar5,y,(int *)CONCAT22(unaff_SS,&local_a));
      if ((7 < local_a) ||
         ((in_DX = 0x3c04, (*(byte *)(y * 0xe0 + iVar5 * 2 + 1) & 0xf) != current_player &&
          (local_a != 0)))) break;
      local_8 = 1;
      param_2 = iVar5;
      param_3 = y;
    }
    iVar3 = dice(1,3,-2);
    iVar5 = iVar5 + iVar3;
    in_DX = dice(1,3,-2);
    in_DX = y + in_DX;
    local_c = local_c + -1;
    y = in_DX;
  } while (local_c != 0);
  return (int *)0x0;
}


