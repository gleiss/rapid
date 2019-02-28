func main()
{
	Int[] a;
	const Int[] b;
	const Int blength;
	const Int k;

	Int i = 0;
	while(i < k)
	{
		a[i] = b[i];
		i = i + 1;
	}
}

(assert-not
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
