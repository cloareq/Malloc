NAME		=	libmy_malloc_$(HOSTTYPE).so

SRCS		=	malloc.c

OBJS		=	$(SRCS:.c=.o) 

CFLAGS		+=	-Wall -Wextra -Werror -fPIC

LDFLAGS		=	-shared	


all		:	$(NAME)

$(NAME)		:	$(OBJS)
			gcc $(OBJS) $(LDFLAGS) -o $(NAME)
			ln -s $(NAME) libmy_malloc.so
clean		:	
			rm -f $(OBJS)

fclean		:	clean
			rm -f $(NAME)
			rm -f libmy_malloc.so

re		:	fclean all
