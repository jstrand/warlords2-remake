
undefined2 __cdecl16far
FUN_57ea_177f(int param_1,int *param_2,int *param_3,undefined2 *param_4,undefined4 param_5,
             undefined4 param_6)

{
  char cVar1;
  int x2;
  int y2;
  int x2_00;
  int y2_00;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int y1;
  bool bVar6;
  int local_1a;
  int local_10;
  int local_e;
  undefined2 local_4;
  
  local_4 = 0;
  x2 = *(int *)(param_1 * 0x16 + -0x7780);
  y2 = *(int *)(param_1 * 0x16 + -0x777e);
  x2_00 = *(int *)(*(char *)(param_1 * 0x16 + -0x7777) * 0x41 + 0x157d);
  y2_00 = *(int *)(*(char *)(param_1 * 0x16 + -0x7777) * 0x41 + 0x157f);
  local_e = 0;
  bVar6 = (*(uint *)(param_1 * 0x16 + -0x7774) & 0x1000) != 0;
  *param_4 = 0;
  iVar2 = dice(1,4,-1);
  if ((((iVar2 == 0) && (*(char *)(*(byte *)(y2 * 0xe0 + x2 * 2) + 0x710) == '\n')) &&
      (iVar2 = FUN_1555_109d(x2), iVar2 != 0)) && (*(char *)(param_1 * 0x16 + -0x777c) != '\x1c')) {
    bVar6 = true;
  }
  iVar2 = x2 + -5;
  do {
    if (x2 + 5 <= iVar2) {
      return local_4;
    }
    for (y1 = y2 + -5; y1 < y2 + 5; y1 = y1 + 1) {
      if (((-1 < iVar2) && (-1 < y1)) && ((iVar2 < 0x70 && (y1 < 0x9c)))) {
        if ((*(char *)(*(byte *)(y1 * 0xe0 + iVar2 * 2) + 0x710) == '\n') &&
           ((*(byte *)(y1 * 0xe0 + iVar2 * 2 + 1) & 0xf) != current_player)) {
          FUN_57ea_1bab(param_1,iVar2,y1,param_4,param_5,param_6);
        }
        if (*(char *)(*(byte *)(y1 * 0xe0 + iVar2 * 2) + 0x710) == '\v') {
          FUN_57ea_1c9d(param_1,iVar2,y1,param_4,param_5,param_6);
        }
        if ((*(char *)(*(byte *)(y1 * 0xe0 + iVar2 * 2) + 0x710) != '\n') &&
           (((((DAT_451b_1e9c == 2 || ((*(byte *)(y1 * 0x70 + iVar2) & 0x1f) != 0)) ||
              ((DAT_451b_1e9a & 1) != 0)) ||
             (*(char *)(*(byte *)(y1 * 0xe0 + iVar2 * 2) + 0x710) != '\x05')) &&
            (((*(byte *)(y1 * 0x70 + iVar2) & 0x20) == 0 &&
             ((DAT_451b_1e9c == 2 ||
              (*(int *)((undefined1 *)&terrain_move_cost +
                       *(char *)(*(byte *)(y1 * 0xe0 + iVar2 * 2) + 0x710) * 2) != 0)))))))) {
          local_10 = 0;
          for (iVar5 = iVar2 + -1; iVar5 <= iVar2 + 1; iVar5 = iVar5 + 1) {
            for (iVar4 = y1 + -1; iVar4 <= y1 + 1; iVar4 = iVar4 + 1) {
              if (((-1 < iVar5) && (-1 < iVar4)) &&
                 ((iVar5 < 0x70 &&
                  ((iVar4 < 0x9c && ((*(byte *)(iVar4 * 0x70 + iVar5) & 0x20) == 0)))))) {
                local_10 = local_10 + 1;
              }
            }
          }
          if (1 < local_10) {
            local_1a = 0;
            cVar1 = *(char *)(*(byte *)(y1 * 0xe0 + iVar2 * 2) + 0x710);
            if (bVar6) {
              if ((cVar1 == '\x02') || (cVar1 == '\x03')) {
LAB_57ea_1b01:
                local_1a = local_1a + 200;
              }
            }
            else if ((cVar1 != '\x02') && (cVar1 != '\x03')) {
              if (cVar1 != '\x05') {
                local_1a = 100;
              }
              if ((*(byte *)(y1 * 0x70 + iVar2) & 0x1f) != 0) goto LAB_57ea_1b01;
            }
            iVar5 = map_distance(iVar2,y1,x2,y2);
            iVar3 = map_distance(iVar2,y1,x2_00,y2_00);
            iVar4 = dice(1,10,0);
            iVar5 = iVar4 + iVar5 * 2 + iVar3 + local_10 * 10 + local_1a;
            if ((bVar6) && (local_1a != 0)) {
              if (0x32 < iVar3) {
                iVar3 = 0x32;
              }
              iVar5 = iVar5 + iVar3 * 10;
            }
            if (local_e < iVar5) {
              *param_2 = iVar2;
              *param_3 = y1;
              local_4 = 1;
              local_e = iVar5;
            }
          }
        }
      }
    }
    iVar2 = iVar2 + 1;
  } while( true );
}


