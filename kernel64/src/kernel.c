void halt() {
  while (1);
}

void kernel_main()
{
  // hang
  halt();
}
