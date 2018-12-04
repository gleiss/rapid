//int [] a, b;
//int i, blength, k;
//
//requires 0 <= blength;
//
//requires k <= blength;
//requires i == 0;
//ensures forall int j, 0 <= j & j < k ==> a[j] == b[j];
//
//while (i < k) do
//:: true -> a[i] = b[i]; i = i + 1;
//od

func main()
{
	Int[] a;
	Int[] b;
	Int i;
	Int blength;
	Int k;

	i = 0;
	while(i < k)
	{
		a[i] = b[i];
		i = i + 1;
	}
}

(assert-not
	(=>
		(<= (k main_end) (blength main_end))
		(forall ((j Int))
			(=>
				(and
					(<= 0 j)
					(< j (k main_end))
				)
				(= (a main_end j) (b main_end j))
			)
		)
	)
)