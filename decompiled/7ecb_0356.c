
void __cdecl16far dice_seed(void)

{
  time_t *seed;
  time_t *unaff_SS;
  time_t local_6;
  
  seed = time(&local_6,unaff_SS);
  srand((uint)seed);
  return;
}


