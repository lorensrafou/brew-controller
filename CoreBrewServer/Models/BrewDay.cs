using System;
using System.Collections.Generic;

namespace CoreBrewServer.Models
{
    public class BrewDay
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public DateTime StartTime { get; set; }
        public DateTime ReachedMashTemp {get; set;}
        public DateTime CompletedMash {get; set;}
        public DateTime ReachedBoil {get; set;}
        public DateTime CompletedBoil {get; set;}
        public int BrewScheduleId {get; set;}
        public virtual BrewSchedule BrewSchedule { get; set; }

        public virtual List<MashTemperature> MashTemperatures { get; set; } = new List<MashTemperature>();
    }
}