
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far path_trace(int *param_1)

{
  byte bVar1;
  int iVar2;
  int in_CX;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  byte *pbVar7;
  int *piVar8;
  int *piVar9;
  int unaff_DI;
  undefined2 uVar10;
  undefined2 uVar11;
  uint local_20;
  uint local_1e;
  uint local_12;
  int local_10;
  int local_c;
  int local_a;
  int local_8;
  int local_6;
  int local_4;
  
  local_4 = *param_1;
  uVar10 = (undefined2)((ulong)param_1 >> 0x10);
  piVar8 = (int *)param_1;
  local_6 = piVar8[1];
  uVar4 = *(uint *)((int)_DAT_451b_001a + local_4 * 0x138 + local_6 * 2);
  uVar3 = (int)uVar4 >> 0xf;
  local_8 = (uVar4 ^ uVar3) - uVar3;
  local_10 = 0;
  do {
    local_20 = 0;
    if ((piVar8[2] == local_4) && (piVar8[3] == local_6)) {
      return;
    }
    if (piVar8[5] == 1) {
      local_20 = (uint)*(byte *)((int)path_cost_grid + local_4 * 0x9c + local_6);
      local_1e = local_20 & 8;
      local_20 = local_20 & 0x10;
    }
    local_c = -1;
    local_a = -1;
    iVar2 = direction_between(unaff_DI,in_CX,piVar8[2],piVar8[3]);
    piVar9 = (int *)&DAT_4125_0212;
    do {
      uVar4 = (iVar2 + *piVar9) % 8;
      unaff_DI = local_4 + *(int *)(uVar4 * 2 + 0x222);
      in_CX = local_6 + *(int *)(uVar4 * 2 + 0x232);
      if ((((DAT_4125_00e0 == 0) || ((uVar4 & 1) == 0)) ||
          (((*(byte *)((int)path_cost_grid + unaff_DI * 0x9c + in_CX) & 8) != 0 &&
           (*(char *)(*(byte *)(in_CX * 0xe0 + unaff_DI * 2) + 0x710) != '\x01')))) &&
         ((((-1 < unaff_DI && (unaff_DI < 0x70)) && (-1 < in_CX)) && (in_CX < 0x9c)))) {
        uVar11 = (undefined2)((ulong)_DAT_451b_001a >> 0x10);
        iVar6 = (int)_DAT_451b_001a + unaff_DI * 0x138;
        if ((*(int *)(iVar6 + in_CX * 2) != 30000) && (*(int *)(iVar6 + in_CX * 2) != 0x7531)) {
          if (local_20 == 0) {
            pbVar7 = (byte *)((int)path_cost_grid + unaff_DI * 0x9c + in_CX);
            bVar1 = *pbVar7;
            if ((((bVar1 & 0x10) == 0) && (piVar8[5] == 1)) &&
               (((local_1e != 0 && ((bVar1 & 8) == 0)) || ((local_1e == 0 && ((*pbVar7 & 8) != 0))))
               )) goto LAB_1555_138e;
          }
          uVar3 = *(uint *)(iVar6 + in_CX * 2);
          uVar5 = (int)uVar3 >> 0xf;
          iVar6 = (uVar3 ^ uVar5) - uVar5;
          if ((iVar6 < local_8) || (((DAT_4125_00e0 != 0 && (local_a == -1)) && (iVar6 == local_8)))
             ) {
            local_12 = uVar4;
            local_c = in_CX;
            local_a = unaff_DI;
            local_8 = iVar6;
          }
        }
      }
LAB_1555_138e:
      piVar9 = piVar9 + 1;
    } while (piVar9 != (int *)0x222);
    if ((local_10 < 0xc6) && (local_a != -1)) {
      local_4 = local_a;
      local_6 = local_c;
      *(undefined1 *)((int)*(undefined4 *)(piVar8 + 6) + local_10) = (undefined1)local_12;
      local_10 = local_10 + 1;
    }
    else {
      piVar8[2] = local_4;
      piVar8[3] = local_6;
    }
  } while( true );
}


