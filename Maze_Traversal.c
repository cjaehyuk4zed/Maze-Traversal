#include <stdio.h>
#include <stdlib.h>

// Maze traversal using Dijkstra algorithm by Jaehyuk Chang
// Locate 'intersection points' in the maze and mark them as 'nodes'
// Use these nodes to run a simple dijkstra algorithm, instead of traversing the entire maze!

typedef struct maze_struct {
    char c; // Character in matrix marked with '*' or ' '
    int n; // Node number of the corresponding intersection, initial value is -1
}maze_struct;

typedef struct node_struct {
    char visited; // 't' for visited, 'f' for not visited
    int* child; // maximum 4 child nodes, save node numbers in array
    int* dist; // distance till child nodes corresponding to each saved node in the array
    int* coords; // x, y coords of current node
}node_struct;

typedef struct best { // solution : best path, length, number of nodes in path
    int *path;
    int len;
    int node_count;
}best;

// node list, current_node_number, final_node_number, number_of_nodes, current_dist, path, solution_dist/path))
void dijkstra(node_struct* node, int node_num, int cur_node, int dist, int node_count, int* path, best *solution){
    if(cur_node == node_num){ // if final node has been reached
        if(dist < solution->len){
            path[node_count] = cur_node;
            solution->node_count = node_count+1;
            solution->len = dist+1; // dist is the new minimum distance, +1 to include starting point
            for(int i = 0; i < solution->node_count; i++){ // save minimum distance path
                solution->path[i] = path[i];
            }
        }
    }

    else {
        node[cur_node].visited = 't'; // mark current node as 'visited'
        for(int i = 0; i < 4; i++){
            if(node[cur_node].child[i] != -1){
                if(node[node[cur_node].child[i]].visited == 'f'){
                    path[node_count] = cur_node;
                    dijkstra(node, node_num, node[cur_node].child[i], dist + node[cur_node].dist[i], node_count + 1, path, solution);
                }
            }
        }
        node[cur_node].visited = 'f'; // unmark current node for recursion
    }
}

int main(void)
{
    // User input for name of file to be opened, e.g. [map.txt] but without the brackets
    char file_name[64];
    printf("ENTER FILE NAME (no longer than 64 characters, must be in the same directory as source code)\n");
    printf("For example, [map.txt] - but without the brackets: \n");
    scanf("%s", &file_name);
    printf("\n");

    // Maze file layout :
    /*
    9 10        row_size    column_size
    **********  maze layout
    *     *  *
    * ****** *
    *       **
    ** **   **
    *    *  **
    *  **    *
    *        *
    **********  start_coords finish_coords
    1 1 6 7     row col row col
    */
   
    FILE *fp;
    fp = fopen(file_name, "r");

    // Read number of rows and columns from first line of file
    int row, col;
    fscanf(fp, "%d" "%d", &row, &col);
    fgetc(fp);

    if (fp == NULL){
        printf("Error opening maze file.\n");
        return 1;
    }
    
    // Read file, input data into struct matrix
    maze_struct maze[row+1][col+1];
    char buffer[col+1];
    for(int i = 0; i < row; i++){
        if(!feof(fp) && !ferror(fp)){fgets(buffer, col+2, fp);}
        for(int j = 0; j < col+2; j++){maze[i][j].c = buffer[j];}
    }

    // Coords for start node / finish node
    int start_row, start_col, fin_row, fin_col;
    fscanf(fp, "%d" "%d" "%d" "%d", &start_row, &start_col, &fin_row, &fin_col);
    fclose(fp);

    // Locate intersection points in the maze and mark them as 'nodes'
    // Use these nodes to run a simple dijkstra algorithm, instead of traversing the entire maze!
    char l, r, t, b;
    int tn, bn, ln, rn;
    int node_num = 1; // node_num 0 will be the starting point
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            if(maze[i][j].c == '*'){continue;} // Check for wall node first

            // top, bottom, left right
            if(i!=0){t = maze[i-1][j].c;} else{t = '*';}
            if(i!=(row-1)){b = maze[i+1][j].c;} else{b = '*';}
            if(j!=0){l = maze[i][j-1].c;} else{l = '*';}
            if(j!=(col-1)){r = maze[i][j+1].c;} else{r = '*';}

            // tn, bn, ln, rn (top, bottom, left, right) used to check if there is a wall in their respective directions
            // Similar to chmod permissions in linux, tn = 8, bn = 4, ln = 2, rn = 1 if wall exists, set to 0 if path
            if(t == '*') {tn = 8;} else{tn = 0;}
            if(b == '*') {bn = 4;} else{bn = 0;}
            if(l == '*') {ln = 2;} else{ln = 0;}
            if(r == '*') {rn = 1;} else{rn = 0;}

            int sum = tn + bn + ln + rn;
            // Check if node is an intersection
            if(sum == 0 || sum == 1 || sum == 2 || sum == 4 || sum == 8 || sum == 5 || sum == 6 || sum == 9 || sum == 10){
                maze[i][j].c = 'n'; // mark as node
                maze[i][j].n = node_num; // node number at this location
                node_num++;
            }
        }
    }

    // Note that this may overwrite node_num of an existing intersection!
    // e.g. for the default map.txt, node 1 and 2 are overwritten - no existing data for those two nodes
    maze[start_row][start_col].c = 'n';
    maze[start_row][start_col].n = 0;
    maze[fin_row][fin_col].c = 'n';
    maze[fin_row][fin_col].n = node_num; // node_num = num of final node!

    // Initalise node struct
    node_struct node[node_num+1];
    for(int i = 0; i < node_num + 1; i++){
        node[i].visited = 'f';
        node[i].child = malloc(sizeof(int)*4);
        node[i].dist = malloc(sizeof(int)*4);
        node[i].coords = malloc(sizeof(int)*2);
        node[i].coords[0] = -1;
        node[i].coords[1] = -1;
        for(int j = 0; j < 4; j++){ // Initalise values to -1
            node[i].child[j] = -1;
            node[i].dist[j] = -1;
        }
    }
    
    
    // Check for connected nodes along each 'row'
    int tmp_col = -1;
    for(int i = 0; i < row; i++){
        tmp_col = -1;
        for(int j = 0; j < col; j++){
            if(maze[i][j].c == '*'){tmp_col = -1; continue;}
            if(maze[i][j].c == 'n'){
                if(tmp_col != -1){
                    // int dist = j - tmp_col; node A = maze[i][tmp_col].n; node B = maze[i][j].n
                    for(int k = 0; k < 4; k++){
                        if(node[maze[i][tmp_col].n].child[k] == -1){
                            node[maze[i][tmp_col].n].child[k] = maze[i][j].n;
                            node[maze[i][tmp_col].n].dist[k] = j - tmp_col;
                            node[maze[i][tmp_col].n].coords[0] = i;
                            node[maze[i][tmp_col].n].coords[1] = tmp_col;
                            break;
                        }
                    }
                    for(int k = 0; k < 4; k++){
                        if(node[maze[i][j].n].child[k] == -1){
                            node[maze[i][j].n].child[k] = maze[i][tmp_col].n;
                            node[maze[i][j].n].dist[k] = j - tmp_col;
                            node[maze[i][j].n].coords[0] = i;
                            node[maze[i][j].n].coords[1] = j;
                            break;
                        }
                    }
                    tmp_col = j;
                }
                if(tmp_col == -1){tmp_col = j;}
            }
        }
    }

    // Check for connected nodes along each 'column'
    int tmp_row = -1;
    for(int j = 0; j < col; j++){
        tmp_row = -1;
        for(int i = 0; i < row; i++){
            if(maze[i][j].c == '*'){tmp_row = -1; continue;}
            if(maze[i][j].c == 'n'){
                if(tmp_row != -1){
                    // int dist = j - tmp_col; node A = maze[i][tmp_col].node_num; node B = maze[i][j].node_num
                    for(int k = 0; k < 4; k++){
                        if(node[maze[tmp_row][j].n].child[k] == -1){
                            node[maze[tmp_row][j].n].child[k] = maze[i][j].n;
                            node[maze[tmp_row][j].n].dist[k] = i - tmp_row;
                            node[maze[tmp_row][j].n].coords[0] = tmp_row;
                            node[maze[tmp_row][j].n].coords[1] = j;
                            break;
                        }
                    }
                    for(int k = 0; k < 4; k++){
                        if(node[maze[i][j].n].child[k] == -1){
                            node[maze[i][j].n].child[k] = maze[tmp_row][j].n;
                            node[maze[i][j].n].dist[k] = i - tmp_row;
                            node[maze[i][j].n].coords[0] = i;
                            node[maze[i][j].n].coords[1] = j;
                            break;
                        }
                    }
                    tmp_row = i;
                }
                if(tmp_row == -1){tmp_row = i;}
                maze[i][j].c = ' ';
            }
        }
    }
    
    /* // Test - Print list of connected nodes
    for(int i = 0; i < node_num+1; i++){
        printf("Node: %d, C_Nodes : %d %d %d %d\n", i, node[i].child[0], node[i].child[1], node[i].child[2], node[i].child[3]);
        printf("Node: %d, N_Dist : %d %d %d %d\n", i, node[i].dist[0], node[i].dist[1], node[i].dist[2], node[i].dist[3]);
    }
    */

    int path_max = (row*col)/2;
    int *path = malloc(sizeof(int)*path_max); // node_num of nodes in the path
    path[0] = 0; // node_num of starting node is 0 , node_num of finish node is node_num
    int node_count = 0;

    // Initialise best(path, len, count)
    struct best *solution = malloc(sizeof(struct best));
    solution->path = malloc(sizeof(int)*path_max);
    solution->len = path_max;
    solution->node_count = 0;
    
    if(start_row == fin_row && start_col == fin_col){ // If starting point = finish point, end code here
        for (int i = 0; i < row; i++){
            for(int j = 0; j < col; j++){
                printf("%c", maze[i][j].c);
            }
            printf("\n");
        }
        printf("Starting point is equal to the finishing point\n");
        free(path);
        free(solution);
        return 0;
    }
    else {dijkstra(node, node_num, 0, 0, 0, path, solution);}
    free(path);

    /* // Test - print nodes in the solution path, and the minimum distance
    printf("solution.len : %d\n", solution->len);
    for(int i = 0; i < solution->node_count; i++){
        printf("%d ", solution->path[i]);
    }
    */
    
    // Final step - add path to map, then print map
    for(int i = 0; i < solution->node_count - 1; i++){
        int diff = 0;
        if(node[solution->path[i]].coords[0] < node[solution->path[i+1]].coords[0]){
            diff = node[solution->path[i+1]].coords[0] - node[solution->path[i]].coords[0];
            for(int j = 0; j <= diff; j++){
                maze[node[solution->path[i]].coords[0] + j][node[solution->path[i]].coords[1]].c = 'X';
            }
        }

        if(node[solution->path[i]].coords[0] > node[solution->path[i+1]].coords[0]){
            diff = node[solution->path[i]].coords[0] - node[solution->path[i+1]].coords[0];
            for (int j = 0; j <= diff; j++){
                maze[node[solution->path[i+1]].coords[0] + j][node[solution->path[i+1]].coords[1]].c = 'X';
            }
        }

        if(node[solution->path[i]].coords[1] < node[solution->path[i+1]].coords[1]){
            diff = node[solution->path[i+1]].coords[1] - node[solution->path[i]].coords[1];
            for(int j = 0; j <= diff; j++){
                maze[node[solution->path[i]].coords[0]][node[solution->path[i]].coords[1] + j].c = 'X';
            }
        }

        if(node[solution->path[i]].coords[1] > node[solution->path[i+1]].coords[1]){
            diff = node[solution->path[i]].coords[1] - node[solution->path[i+1]].coords[1];
            for(int j = 0; j <= diff; j++){
                maze[node[solution->path[i+1]].coords[0]][node[solution->path[i+1]].coords[1] + j].c = 'X';
            }

        }
    }

    // Print final result
    for (int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            printf("%c", maze[i][j].c);
        }
        printf("\n");
    }
    // Print minimum distance, or 'path not found'
    if(solution->node_count == 0){printf("Path not found\n");}
    else{printf("%d\n", solution->len);}   

    free(solution);
    return 0;
}