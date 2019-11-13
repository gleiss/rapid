func main()
{
	Int[] a;
	Int alength = 0;
	const Int[] b;
	const Int blength;
	const Int k;

	while(alength < k)
	{
		a[alength] = b[alength];
		alength = alength + 1;
	}
}

(conjecture
	(=>
		(<= k blength)
		(forall ((j Int))
			(=>
				(and
					(<= 0 j)
					(< j k)
				)
				(= (a main_end j) (b j))
			)
		)
	)
)
