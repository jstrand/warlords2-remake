
void __cdecl16far FUN_1b62_0a03(int param_1,undefined4 *param_2,int param_3)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  undefined2 unaff_SS;
  int local_18 [8];
  int local_8;
  undefined2 local_6;
  undefined2 local_4;
  
  local_8 = 0;
  iVar2 = 0;
  do {
    uVar4 = (undefined2)((ulong)param_2 >> 0x10);
    if (param_1 <= iVar2) {
      if ((0 < param_1) && (local_8 = (int)*(char *)((int)*param_2 + 5), local_8 == 0xf)) {
        local_8 = 8;
      }
      for (iVar2 = 1; iVar2 < param_1; iVar2 = iVar2 + 1) {
        iVar3 = iVar2;
        if (((((*(byte *)((int)((undefined4 *)param_2)[iVar2] + 0x11) <
                *(byte *)((int)((undefined4 *)param_2)[iVar2 + -1] + 0x11)) &&
              (local_18[iVar2] != 0)) && (param_3 != 0)) ||
            (*(byte *)((int)((undefined4 *)param_2)[iVar2 + -1] + 0x11) <
             *(byte *)((int)((undefined4 *)param_2)[iVar2] + 0x11))) ||
           ((*(char *)((int)((undefined4 *)param_2)[iVar2] + 0x11) ==
             *(char *)((int)((undefined4 *)param_2)[iVar2 + -1] + 0x11) &&
            (*(char *)(local_8 * 0x1d + (int)*(char *)((int)((undefined4 *)param_2)[iVar2 + -1] + 4)
                      + 0x60b) <
             *(char *)(local_8 * 0x1d + (int)*(char *)((int)((undefined4 *)param_2)[iVar2] + 4) +
                      0x60b))))) {
          while ((0 < iVar3 &&
                 ((((*(byte *)((int)((undefined4 *)param_2)[iVar3] + 0x11) <
                     *(byte *)((int)((undefined4 *)param_2)[iVar3 + -1] + 0x11) &&
                    (local_18[iVar3] != 0)) && (param_3 != 0)) ||
                  ((*(byte *)((int)((undefined4 *)param_2)[iVar3 + -1] + 0x11) <
                    *(byte *)((int)((undefined4 *)param_2)[iVar3] + 0x11) ||
                   ((*(char *)((int)((undefined4 *)param_2)[iVar3] + 0x11) ==
                     *(char *)((int)((undefined4 *)param_2)[iVar3 + -1] + 0x11) &&
                    (*(char *)(local_8 * 0x1d +
                               (int)*(char *)((int)((undefined4 *)param_2)[iVar3 + -1] + 4) + 0x60b)
                     < *(char *)(local_8 * 0x1d +
                                 (int)*(char *)((int)((undefined4 *)param_2)[iVar3] + 4) + 0x60b))))
                   )))))) {
            local_4 = *(undefined2 *)((int)((undefined4 *)param_2 + iVar3) + 2);
            local_6 = *(undefined2 *)((undefined4 *)param_2 + iVar3);
            uVar1 = *(undefined2 *)((undefined4 *)param_2 + iVar3 + -1);
            *(undefined2 *)((int)((undefined4 *)param_2 + iVar3) + 2) =
                 *(undefined2 *)((int)((undefined4 *)param_2 + iVar3 + -1) + 2);
            *(undefined2 *)((undefined4 *)param_2 + iVar3) = uVar1;
            *(undefined2 *)((int)((undefined4 *)param_2 + iVar3 + -1) + 2) = local_4;
            *(undefined2 *)((undefined4 *)param_2 + iVar3 + -1) = local_6;
            iVar3 = iVar3 + -1;
          }
        }
      }
      return;
    }
    local_18[iVar2] = 0;
    iVar3 = 0;
    while( true ) {
      if ((DAT_451b_1e9e <= iVar3) || (DAT_4125_2bda == 0)) goto LAB_1b62_0a66;
      if ((*(int *)((int)((undefined4 *)param_2 + iVar2) + 2) ==
           *(int *)((undefined *)&DAT_451b_1ee0 + iVar3 * 4)) &&
         (*(int *)((undefined4 *)param_2 + iVar2) == ((undefined2 *)&DAT_451b_1ede)[iVar3 * 2]))
      break;
      iVar3 = iVar3 + 1;
    }
    local_18[iVar2] = 1;
LAB_1b62_0a66:
    if ((*(int *)((int)((undefined4 *)param_2 + iVar2) + 2) == DAT_451b_1f02._2_2_) &&
       (*(int *)((undefined4 *)param_2 + iVar2) == (int)DAT_451b_1f02)) {
      local_18[iVar2] = 1;
    }
    iVar2 = iVar2 + 1;
  } while( true );
}


