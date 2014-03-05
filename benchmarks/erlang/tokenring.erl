%
% Code by Vasileios Trigonakis
% From: http://trigonakis.com/blog/2011/05/26/introduction-to-erlang-message-passing/
%

-module(tokenring).
-author("V. Trigonakis").
-export([create/1, node/2, connect/1]).
 
-define(MAXVAL, 100000).
 
%creates the ring's nodes, connects them in a ring, sends the token in the ring, and
%collects the exit messages from the nodes
create(NumNodes) when is_integer(NumNodes), NumNodes > 1 ->
    Nodes = [spawn(?MODULE, node, [ID, self()]) || ID <- lists:seq(1, NumNodes)],
    ring:connect(Nodes),
    hd(Nodes) ! {token, 0},
    getexits(Nodes).
 
%collects the exit messages from the nodes
getexits([]) ->
    io:format("[Coord] Done.~n"),
    ok;
getexits(Nodes) ->
    receive
	{Node, exit} ->
	    case lists:member(Node, Nodes) of
		true ->
		    getexits(lists:delete(Node, Nodes));
		_ ->
		    getexits(Nodes)
	    end
    end.
 
%little trick in order to connect the last with the first node
%handle the [nd0, nd1, ..., ndN] list as [nd0, nd1, ..., ndN, nd0]
connect(N = [H | _]) ->
    connect_(N ++ [H]).
 
%connects the nodes to a ring
connect_([]) ->
    connected;
connect_([_]) ->
    connected;
connect_([N1, N2 | Nodes]) ->
    N1 ! {self(), connect, N2},
    connect_([N2 | Nodes]).
 
%the node function. Initially waits for the next node's pid
node(ID, CrdId) ->
    receive
	{CrdId, connect, NxtNdId} ->
	    io:format("[~p:~p] got my next ~p~n", [ID, self(), NxtNdId]),
	    node(ID, CrdId, NxtNdId)
    end.
 
%the main functionality of a node; receive the token, increase its value and send
%it to the next node on the ring
node(ID, CrdId, NxtNdId) ->
    receive
	{token, Val} ->
	    if
		Val < ?MAXVAL ->
		    NxtNdId ! {token, Val + 1},
		    node(ID, CrdId, NxtNdId);
		true ->
		    io:format("[~p:~p] token value ~p~n", [ID, self(), Val]),
		    case erlang:is_process_alive(NxtNdId) of
			true ->
			    NxtNdId ! {token, Val + 1};
			_ ->
			    ok
		    end,
		    CrdId ! {self(), exit},
		    done
	    end
    end.
