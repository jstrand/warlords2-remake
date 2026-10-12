
void __cdecl16far
FUN_7ae8_01af(undefined2 param_1,int param_2,undefined2 param_3,undefined2 param_4)

{
  int iVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  undefined2 in_DX;
  int iVar5;
  undefined2 *puVar6;
  int iVar7;
  undefined2 uVar8;
  undefined4 local_a;
  undefined4 local_6;
  
  piVar2 = (int *)FUN_7ae8_0198(param_1);
  local_6 = (int *)CONCAT22(in_DX,piVar2);
  iVar3 = piVar2[5];
  do {
    do {
      iVar4 = iVar3 + -1;
      if (iVar3 == 0) {
        return;
      }
      iVar3 = iVar4;
    } while (*(int *)((int)*(undefined4 *)(piVar2 + 2) + iVar4 * 8) != param_2);
    local_a = (undefined4 *)
              CONCAT22(piVar2[1],
                       (undefined4 *)
                       (*local_6 + *(int *)((int)*(undefined4 *)(piVar2 + 2) + iVar4 * 8 + 2) * 0x2e
                       ));
    iVar1 = 0;
    iVar4 = *(int *)((int)*(undefined4 *)(piVar2 + 2) + iVar4 * 8 + 4) + -1;
    while (iVar5 = iVar1, *(char *)*(undefined4 *)((int)*local_a + iVar5 * 4) != '\0') {
      if (*(char *)*(undefined4 *)((int)*local_a + iVar5 * 4) == '%') {
        iVar1 = iVar5 + 1;
      }
      else {
        iVar7 = iVar4 + -1;
        if (iVar4 == 0) break;
        iVar1 = iVar5 + 1;
        iVar4 = iVar7;
        if (*(char *)*(undefined4 *)((int)*local_a + (iVar5 + 1) * 4) == '&') {
          iVar1 = iVar5 + 3;
        }
      }
    }
    uVar8 = (undefined2)((ulong)*local_a >> 0x10);
    puVar6 = (undefined2 *)((int)*local_a + iVar5 * 4);
    puVar6[1] = param_4;
    *puVar6 = param_3;
  } while( true );
}


