func main()
{
	Int[] a;
	const Int[] b;
	const Int length;

	Int i = 0;
	while(i < length && b[i] != 0)
	{
		a[i] = b[i];
		i = i + 1;
	}
}

(assert-not
	(forall ((j Int))
		(=>
			(and
				(<= 0 j)
				(< j (i main_end))
			)
			(= (a main_end j) (b j) )
		)
	)
)
