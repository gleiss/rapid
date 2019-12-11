// similar to copy, but in reverse

func main()
{
	Int[] a;
	const Int[] b;
	const Int length;

	Int i = 0;
	while(i < length)
	{
		a[i] = b[length - 1 - i];
		i = i + 1;
	}
}

(conjecture
	(forall ((j Int))
		(=>
			(and
				(<= 0 length)
				(<= 0 j)
				(< j length)
			)
			(= (a main_end j) (b (- (- length 1) j)) )
		)
	)
)
