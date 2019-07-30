namespace CoreBrewServer.Entities
{
    public class Brew 
    {
        public bool running { get; set; }
        public float temperature { get; set; }
        public int brewdayId { get; set; }
    }
}