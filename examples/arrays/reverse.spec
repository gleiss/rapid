// similar to copy, but keeps track of the size of a and asserts the property with respect to the size of a

func main()
{
	Int[] a;
	const Int[] b;
	const Int alength;

	Int i = 0;
	while(i < alength)
	{
		a[i] = b[alength - 1 - i];
		i = i + 1;
	}
}

(conjecture
	(forall ((j Int))
		(=>
			(and
				(<= 0 alength)
				(<= 0 j)
				(< j alength)
			)
			(= (a main_end j) (b (- (- alength 1) j)) )
		)
	)
)
