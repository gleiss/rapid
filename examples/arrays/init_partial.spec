func main()
{
	Int[] a;
	const Int alength;
	const Int k;

	Int i = 0;
	while(i < alength)
	{
		a[i] = 0;
		i = i + 1;
	}
}

(conjecture
   (forall ((pos Int))
      (=>
         (and
            (<= 0 pos)
            (< pos k)
            (<= k alength)
            (<= 0 alength)
         )
         (= (a main_end pos) 0)
      )
   )
)
