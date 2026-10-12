
undefined2 __cdecl16far
FUN_78a8_03ea(undefined1 *param_1,undefined4 param_2,char *param_3,char *param_4)

{
  undefined1 uVar1;
  int fd;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined1 *puVar4;
  uint unaff_DI;
  int iVar5;
  undefined2 uVar6;
  void *unaff_SS;
  char *pcVar7;
  char local_cc [200];
  undefined1 local_4;
  undefined1 local_3;
  
  strcpy((char *)CONCAT22(unaff_SS,local_cc),param_3);
  pcVar7 = strcat((char *)CONCAT22(unaff_SS,local_cc),param_4);
  uVar3 = (undefined2)((ulong)pcVar7 >> 0x10);
  fd = open((char *)CONCAT22(unaff_SS,local_cc),2);
  if (fd != -1) {
    read(fd,(void *)CONCAT22(1,unaff_SS),unaff_DI);
    *param_1 = local_4;
    uVar2 = mem_alloc(local_4,0,0x20,0,0);
    uVar2 = FUN_1d28_075d(uVar2,uVar3);
    uVar6 = (undefined2)((ulong)param_1 >> 0x10);
    puVar4 = (undefined1 *)param_1;
    *(undefined2 *)(puVar4 + 0x13) = uVar3;
    *(undefined2 *)(puVar4 + 0x11) = uVar2;
    if (*(long *)(puVar4 + 0x11) != 0) {
      uVar2 = mem_alloc(local_4,0,0x20,0,0);
      uVar2 = FUN_1d28_075d(uVar2,uVar3);
      *(undefined2 *)(puVar4 + 0x17) = uVar3;
      *(undefined2 *)(puVar4 + 0x15) = uVar2;
      if (*(long *)(puVar4 + 0x15) != 0) {
        read(fd,(void *)CONCAT22(1,unaff_SS),unaff_DI);
        puVar4[1] = local_3;
        uVar3 = FUN_78a8_0b9d(fd);
        *(undefined2 *)(puVar4 + 2) = uVar3;
        *(undefined4 *)(puVar4 + 4) = param_2;
        read(fd,(void *)CONCAT22(1,unaff_SS),unaff_DI);
        puVar4[8] = local_3;
        read(fd,(void *)CONCAT22(1,unaff_SS),unaff_DI);
        puVar4[9] = local_3;
        read(fd,(void *)CONCAT22(1,unaff_SS),unaff_DI);
        puVar4[10] = local_3;
        uVar3 = FUN_78a8_0b9d(fd);
        *(undefined2 *)(puVar4 + 0xb) = uVar3;
        uVar2 = (undefined2)
                ((ulong)((long)*(int *)(puVar4 + 0xb) * (long)(int)(uint)(byte)puVar4[8]) >> 0x10);
        *(int *)(puVar4 + 0xd) =
             (int)((long)*(int *)(puVar4 + 0xb) * (long)(int)(uint)(byte)puVar4[8]);
        uVar3 = FUN_78a8_0b9d(fd);
        *(undefined2 *)(puVar4 + 0xf) = uVar3;
        uVar1 = *param_1;
        read(fd,(void *)(ulong)CONCAT12(uVar1,(int)((ulong)*(undefined4 *)(puVar4 + 0x11) >> 0x10)),
             unaff_DI);
        read(fd,(void *)(ulong)CONCAT12(uVar1,(int)((ulong)*(undefined4 *)(puVar4 + 0x15) >> 0x10)),
             unaff_DI);
        uVar3 = mem_alloc(*(int *)(puVar4 + 0xf) * 6,0,0x20,0,0);
        uVar3 = FUN_1d28_075d(uVar3,uVar2);
        *(undefined2 *)(puVar4 + 0x1b) = uVar2;
        *(undefined2 *)(puVar4 + 0x19) = uVar3;
        if (*(long *)(puVar4 + 0x19) != 0) {
          for (iVar5 = 0; iVar5 < *(int *)(puVar4 + 0xf); iVar5 = iVar5 + 1) {
            uVar3 = FUN_78a8_0b9d(fd);
            *(undefined2 *)((int)*(undefined4 *)(puVar4 + 0x19) + iVar5 * 6) = uVar3;
            uVar3 = FUN_78a8_0b9d(fd);
            *(undefined2 *)((int)*(undefined4 *)(puVar4 + 0x19) + iVar5 * 6 + 2) = uVar3;
            uVar3 = FUN_78a8_0b9d(fd);
            *(undefined2 *)((int)*(undefined4 *)(puVar4 + 0x19) + iVar5 * 6 + 4) = uVar3;
          }
          close(fd);
          return 1;
        }
      }
    }
    close(fd);
  }
  return 0;
}


