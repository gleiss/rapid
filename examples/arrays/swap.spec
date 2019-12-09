func main()
{
	Int[] a;
	Int[] b;
	const Int[] olda;
	const Int[] oldb;

	const Int alength;
	const Int blength;
	Int current;

	Int i = 0;
	while(i < alength)
	{
		current = a[i];
		a[i] = b[i];
		b[i] = current;
		i = i + 1;
	}
}

(conjecture
	(forall ((j Int)(k Int))
		(=>
			(and
				(<= 0 j)
				(< j blength)
				(<= 0 k)
				(< k alength)
				(= alength blength)
				(<= 0 alength)
				(= (a (l11 zero) k) (olda k))
				(= (b (l11 zero) k) (oldb k))
			)
			(= (a main_end j) (oldb j))
		)
	)
)

(conjecture
	(forall ((j Int)(k Int))
		(=>
			(and
				(<= 0 j)
				(< j blength)
				(<= 0 k)
				(< k alength)
				(= alength blength)
				(<= 0 alength)
				(= (a (l11 zero) k) (olda k))
				(= (b (l11 zero) k) (oldb k))
			)
			(= (b main_end j) (olda j))
		)
	)
)
