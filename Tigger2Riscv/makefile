all : eeyore tigger riscv64

eeyore : m2e/*
	(cd m2e; make)

tigger : e2t/*
	(cd e2t; make)

riscv64 : t2r/*
	(cd t2r; make)

clean :
	(cd m2e; make clean)
	(cd e2t; make clean)
	(cd t2r; make clean)
