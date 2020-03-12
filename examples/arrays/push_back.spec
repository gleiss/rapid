// similar to copy, but keeps track of the size of a and asserts the property with respect to the size of a

func main()
{
	Int[] a;
	const Int[] b;
	const Int blength;

	Int alength = 0;
	Int i = 0;
	while(i < blength)
	{
		a[i] = b[i];
		alength = alength + 1;
		i = i + 1;
	}
}

(axiom
	(forall ((it Nat))
		(<= (alength (l11 it)) (i (l11 it)))
	)
)

(conjecture
	(forall ((j Int))
		(=>
			(and
				(<= 0 blength)
				(<= 0 j)
				(< j (alength main_end))
			)
			(= (a main_end j) (b j))
		)
	)
)