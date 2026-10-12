
undefined2 * __cdecl16far
city_produce_army(int param_1,undefined2 param_2,undefined2 param_3,int param_4)

{
  char cVar1;
  int iVar2;
  undefined2 *puVar3;
  int in_DX;
  undefined4 local_8;
  
  cVar1 = *(char *)(param_1 * 0x41 + 0x15a9);
  iVar2 = FUN_7087_0000(param_1,(int)cVar1);
  puVar3 = (undefined2 *)FUN_1b62_0309();
  local_8 = (undefined2 *)CONCAT22(in_DX,puVar3);
  if (puVar3 != (undefined2 *)0x0 || in_DX != 0) {
    if (param_4 == -1) {
      *local_8 = param_2;
      puVar3[1] = param_3;
    }
    *(char *)(puVar3 + 2) = cVar1;
    *(undefined1 *)((int)puVar3 + 5) = *(undefined1 *)(param_1 * 0x41 + 0x1592);
    *(undefined1 *)(puVar3 + 3) = *(undefined1 *)(param_1 * 0x41 + iVar2 + 0x159f);
    *(undefined1 *)((int)puVar3 + 7) = *(undefined1 *)(param_1 * 0x41 + iVar2 + 0x159f);
    if (*(int *)(current_player * 2 + 0xf0) == 0) {
      *(undefined1 *)(puVar3 + 4) = *(undefined1 *)(param_1 * 0x41 + iVar2 + 0x159b);
    }
    else {
      if (*(char *)(param_1 * 0x41 + iVar2 + 0x159b) + 2 < 10) {
        cVar1 = *(char *)(param_1 * 0x41 + iVar2 + 0x159b) + '\x02';
      }
      else {
        cVar1 = '\t';
      }
      *(char *)(puVar3 + 4) = cVar1;
    }
    *(undefined1 *)((int)puVar3 + 9) = (undefined1)param_1;
    *(undefined1 *)(puVar3 + 5) = 0;
    *(char *)((int)puVar3 + 0xb) = *(char *)(param_1 * 0x41 + iVar2 + 0x15a3) / '\x02';
    *(undefined1 *)(param_1 * 0x41 + 0x15aa) = 0;
    if (param_4 == -1) {
      *(byte *)((int)puVar3 + 0xf) = *(byte *)((int)puVar3 + 0xf) & 1;
      *(byte *)(puVar3 + 7) = *(byte *)(puVar3 + 7) & 0xf0;
      *(undefined1 *)(puVar3 + 7) = *(undefined1 *)(puVar3 + 7);
      *(undefined1 *)(puVar3 + 8) = 0xff;
      FUN_1b62_02e2(puVar3,in_DX);
    }
    else {
      if (param_4 < 100) {
        *local_8 = 0xffff;
      }
      else {
        *local_8 = 0xfffe;
      }
      puVar3[1] = 0xffff;
      *(byte *)((int)puVar3 + 0xf) = *(byte *)((int)puVar3 + 0xf) & 1;
      *(byte *)((int)puVar3 + 0xf) = *(byte *)((int)puVar3 + 0xf) | (char)param_4 << 1;
      *(undefined1 *)(puVar3 + 8) = 0x65;
    }
  }
  return puVar3;
}


