# ##
t TT
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      iptors");
		return 1;
	}
	for(i = 1; i < nprocs; i++){
		if(pipe(fd) == -1){
			fprintf(stderr, "[%ld]: failed to create pipe %d: %s\n",
				(long)getpid(), i, strerror(errno));
			return 1;
		}
		if((childpid = fork()) == -1){
			fprintf(stderr, "[%ld]: failed to create child %d: %s\n",(long)getpid(), i, strerror(errno));
			return 1;
		}
		if(childpid > 0)
			error = dup2(fd[1], STDOUT_FILENO);
		else
			error = dup2(fd[0], STDIN_FILENO);
		if (error == -1){
			fprintf(stderr, "[%ld]: failed to close extra descriptors %d: %s\n",
				(long)getpid(), i, strerror(errno));
			return 1;
		}
		if ((close(fd[0]) == -1 || close (fd[1]) == -1)) {
			fprintf(stderr, "[%ld]: failed to close extra descriptors %d: %s\n",
				(long)getpid(), i, strerror(errno));
			return 1;
		}
		if(childpid)
			 break;
	}
	if(i == 1){
		write(STDOUT_FILENO, "y", 1);
		read(STDIN_FILENO, t, 1);
	}
	sprintf(s, "This is process %d with ID equal to %ld and parent id %ld\n",
		i, (long)getpid(), (long)getppid());
	if(i != 1)
		read(STDIN_FILENO, t, 1);
	srand48((long)getpid());
	drand = drand48()*atoi(argv[3]);
	fprintf(stderr, "%f\n",drand);
	while(drand-- > 0)
		prtastr(s, STDERR_FILENO, atoi(argv[2]));
	write(STDOUT_FILENO, "y", 1);
	return 0;
}
