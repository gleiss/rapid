func main()
{
	Int[] a;
	const Int[] b;
	const Int blength;

	Int i = 0;
	while (i < (blength / 2))
	{
		a[i] = b[i*2];
		i = i + 1;
	}
}

(assert-not
	(forall ((k Int))
		(=>
			(and
				(<= 0 k)
				(< k (/ blength 2) )
				(<= 0 blength)
			)
			(= (a main_end k) (b main_end (* k 2)))
		)
	)
)
