bool isOnBoundary(Map::Vertex* v ) {
        Map::Halfedge* h = v->halfedge();
        Map::Halfedge* tmp = h;
        do {
            if (tmp->facet() == 0) {
                return 1;
            }
            tmp = tmp->opposite()->prev();
        } while(tmp != h);
        return 0;
    }


    void SurfaceQuickStartTool::grab(const RayPick& p_ndc) {
        Map::Vertex* v = picker()->pick_vertex(p_ndc) ;
        if(v == nil) {
	        status("did not pick any vertex") ;
        } else {
            // Find nearest boundary
            MapVertexAttribute<bool> aVisit(surface(), "visited");
            MapVertexAttribute<int> aDist(surface(), "distance");
            // reset attributes from previous click
            FOR_EACH_VERTEX(Map, surface(), it) {
                aVisit[it] = 0;
                aDist[it] = 0;
            }

            std::deque<Map::Vertex*> q;
            q.push_back(v);
            while (!q.empty()) {
                // Visit vertex
                v = q.front();
                q.pop_front();
                aVisit[v] = 1;
                if (isOnBoundary(v)) {
                    char buff[100];
                    sprintf(buff, "The nearest boundary is %d edges away", aDist[v]);
                    status(buff);
                    return;
                }

                // Add unvisited neighbors to queue and continue
                Map::Halfedge* h = v->halfedge();
                Map::Halfedge* tmp = h;
                // distance from our visited vertex to this neighbor
                int distance = aDist[v] + 1;
                do {
                    Map::Vertex* neighbor = tmp->opposite()->vertex();

                    
                    // skip if this neighbor is already visited
                    if (!aVisit[neighbor]) {
                        // check if vertex is in queue already
                        int i = 0;
                        while (i < q.size()) {
                            if (q.at(i) == neighbor) {
                                break; // vertex found
                            }
                            ++i;
                        }
                        if (i == q.size()) {
                            // vertex not in queue
                            aDist[neighbor] = distance;
                            q.push_back(neighbor);
                        } else {
                            // vertex already in queue, compare distances
                            if (distance < aDist[neighbor]) {
                                aDist[neighbor] = distance;
                            }
                        }
                    }
                    tmp = tmp->opposite()->prev();
                } while (tmp != h);
            }
            status("No boundries found");
            return;
        }
    }
