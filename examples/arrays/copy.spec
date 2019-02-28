func main()
{
	Int[] a;
	const Int[] b;
	Int i;
	const Int blength;

	i = 0;
	while(i < blength)
	{
		a[i] = b[i];
		i = i + 1;
	}
}

(assert-not
	(forall ((j Int))
		(=>
			(and
				(<= 0 blength)
				(<= 0 j)
				(< j blength)
			)
			(= (a main_end j) (b j))
		)
	)
)
