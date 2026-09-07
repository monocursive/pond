defmodule PondWeb.Router do
  use Phoenix.Router

  pipeline :api do
    plug(:accepts, ["json"])
  end

  scope "/", PondWeb do
    pipe_through(:api)
    get("/health", APIController, :health)
    get("/ready", APIController, :ready)
    post("/v1/installations", APIController, :register)
    post("/v1/drops", APIController, :drop)
    get("/v1/pond", APIController, :pond)
    delete("/v1/installation", APIController, :leave)
  end
end
