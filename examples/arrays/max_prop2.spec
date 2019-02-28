func main()
{
	Int[] a;
   const Int[] b;
	const Int blength;

	Int i = 0;
	while(i < blength)
	{
      if(b[i] >= 0)
      {
         a[i] = b[i];
      }
      else
      {
         a[i] = 0;
      }
		i = i + 1;
	}
}

(assert-not
   (forall ((pos Int))
      (=>
         (and
            (<= 0 pos)
            (< pos blength)
            (<= 0 blength)
         )
         (or
            (= (a main_end pos) 0)
            (= (a main_end pos) (b pos))
         )
      )
   )
)
