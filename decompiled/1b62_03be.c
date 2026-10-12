
void __cdecl16far FUN_1b62_03be(undefined2 *param_1)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  
  uVar2 = (undefined2)((ulong)param_1 >> 0x10);
  puVar1 = (undefined2 *)param_1;
  *(undefined1 *)(puVar1 + 2) = 0xff;
  *(undefined1 *)((int)puVar1 + 5) = 0xff;
  *param_1 = 0xffff;
  puVar1[1] = 0xffff;
  *(byte *)(puVar1 + 7) = *(byte *)(puVar1 + 7) & 0x8f;
  *(byte *)((int)puVar1 + 0xf) = *(byte *)((int)puVar1 + 0xf) & 1;
  *(undefined1 *)((int)puVar1 + 0xf) = *(undefined1 *)((int)puVar1 + 0xf);
  *(byte *)(puVar1 + 7) = *(byte *)(puVar1 + 7) & 0xf0;
  *(undefined1 *)(puVar1 + 7) = *(undefined1 *)(puVar1 + 7);
  *(undefined1 *)(puVar1 + 8) = 0xff;
  *(undefined1 *)((int)puVar1 + 7) = 0;
  return;
}


